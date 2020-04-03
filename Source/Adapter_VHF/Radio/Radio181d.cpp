#include "Radio181d.h"
#include "RadioManage.h"
#include <stdint.h>
#include <QDebug>
#include <QDateTime>
#include "socket/socketcommon.h"

Radio181D::Radio181D()
{
     Protocol = 1;
}

Radio181D::~Radio181D()
{

}

void Radio181D::serialInit()
{
#if WIN32
    dataCom = new QextSerialPort("COM1");
#else
    dataCom = new QextSerialPort("/dev/ttymxc1");
#endif
    connect(dataCom, SIGNAL(readyRead()), this, SLOT(readDataCom()));
    dataCom->setBaudRate(BAUD38400);    //设置波特率
    dataCom->setDataBits(DATA_8);       //设置数据位
    dataCom->setParity(PAR_NONE);       //设置校验
    dataCom->setStopBits(STOP_1);       //设置停止位
    dataCom->setFlowControl(FLOW_OFF);  //设置数据流控制
    dataCom->setTimeout(10);            //设置延时
    //
    if (false == dataCom->open(QIODevice::ReadWrite))
    {
        qDebug() << "181D Data Serail Open Err!";
    } else {
        qDebug() << "181D Data Serail Open Success!";
    }

#if WIN32
    ctrlCom = new QextSerialPort("COM2");
#else
    ctrlCom = new QextSerialPort("/dev/ttymxc2");
#endif

    connect(ctrlCom, SIGNAL(readyRead()), this, SLOT(readCtrlCom()));
    ctrlCom->setBaudRate(BAUD38400);    //设置波特率
    ctrlCom->setDataBits(DATA_8);       //设置数据位
    ctrlCom->setParity(PAR_NONE);       //设置校验
    ctrlCom->setStopBits(STOP_1);       //设置停止位
    ctrlCom->setFlowControl(FLOW_OFF);  //设置数据流控制
    ctrlCom->setTimeout(10);           //设置延时
    //
    if (false == ctrlCom->open(QIODevice::ReadWrite))
    {
        qDebug() << "181D Ctrl Serail Open Err!";
    } else {
        qDebug() << "181D Ctrl Serail Open Success 100ms!";
    }

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(2000);

    updTim = QDateTime::currentDateTimeUtc().toTime_t();                     //秒级

    memset(&radioState, 0, sizeof(VHF_ACK_STATE));
}

void Radio181D::readDataCom()
{
    QByteArray data = dataCom->readAll();
    if(!data.isEmpty())
    {

        qDebug() << "181D DataCom Recv size: " << data.size();
        //emit comRecData(temp);
        RadioManage::getInstance()->onRecvLinkData(data);
    }
}

void Radio181D::readCtrlCom()
{
    dataArray.push_back(ctrlCom->readAll());

    if (dataArray.length() < 7)
    {
        qDebug() << "Recv data length too short! ";
        return;
    }
    else
    {
        // eg. 16H 10H 02H + 数据段 + 10H 03H HH LL(HH LL为2Byte占位)
        // 数据段读入需要把 10H 10H 转为 10H
        int  step = 0;  //
        char sd[3];     // 16H 10H 02H
        int  contBegin = 0;
        int  contLen = 0;
        int  removeLen = 0;
        for(int i=0; i< dataArray.length(); i++)
        {
            char currd = dataArray.at(i);
            if ( step == 0 )//定位包头
            {

                sd[0] = sd[1];
                sd[1] = sd[2];
                sd[2] = currd;

                // 16H 10H 02H
                if(sd[0] == 0x16 && sd[1] == 0x10 && sd[2] == 0x02)
                {
                    step = 1;
                    contBegin = i+1;
                    contLen = 0;
                    memset(sd ,0 , sizeof(sd));
                }
            }
            else if ( step == 1 )//定位包尾
            {
                sd[0] = sd[1];
                sd[1] = currd;
                contLen += 1;

                if(sd[0] == 0x10 && sd[1] == 0x03)
                {
                    //判断数据包剩余长度是否满足校验占位
                    if ((dataArray.length()-3-contLen) < 2)
                    {
                        qDebug() << "Package end length too short!";
                        return;
                    }
                    step = 2;
                    contLen -= 2;
                    memset(sd, 0, sizeof(sd));
                }
            }
            else if ( step == 2 )//解析数据内容
            {
                char tmp[50];
                memset(tmp, 0, sizeof(tmp));
                //取出数据内容
                memcpy(tmp, dataArray.mid(contBegin, contLen).data(), contLen);

                char  state[50];
                memset(state, 0, sizeof(state));
                int   stateLen = 0;

                for (int m = 0; m < contLen; ++m)
                {
                    if (tmp[m] != 0x10)
                    {
                        state[stateLen] = tmp[m];
                        ++stateLen;
                    }
                    else
                    {
                        if (m < contLen-1 && tmp[m+1] == 0x10)
                        {
                            state[stateLen] = 0x10;
                            ++stateLen;
                            m += 1;
                        }
                        else
                        {
                            qDebug() << "Recv data content Err!";
                            memset(state, 0, sizeof(state));
                            stateLen = 0;
                            break;
                        }
                    }
                }
                //解包到正确数据，发出该数据
                if (stateLen > 0)
                    updateRadioState(state, stateLen);
                //更新已解码成功的数据长度
                removeLen = contBegin + contLen + 4;

                step = 0;
                contLen = 0;
            }
        }

        dataArray.remove(0, removeLen);
    }
}


void Radio181D::updateRadioState(char* data, int len)
{

    char funCod = data[0];
    if((unsigned char)funCod == 0xF8 || (unsigned char)funCod == 0xF8 - 0x10){                //输出的电台查询状态
        if(len != 9){
            return;
        }

        char  workMod  = data[1];
        float sndFreq = float(400.0+double(data[2]*256 + data[3])*0.025);
        float revFreq = float(400.0+double(data[4]*256 + data[5])*0.025);
        char  channel = data[6];
        char  signalV = data[7];
        char  intensityEx = data[8];

        radioState.funCod  = 0xF8;
        radioState.workMod = workMod;
        radioState.channel = channel;
        radioState.signalV = signalV;
        radioState.radioPro= intensityEx;

        char ackData[sizeof(VHF_ACK_STATE)];
        memcpy(ackData, &radioState, sizeof(VHF_ACK_STATE));
        RadioManage::getInstance()->onCtrlAck(MessageTyp_VHF_Ack_State, ackData, sizeof(VHF_ASK_STATE));

    }

    if((unsigned char)funCod == 0xF4 || (unsigned char)funCod == 0xF4 - 0x10){                //输出的语音/数据状态(工作模式)

        if(len != 3){
            return;
        }

        char wkM1 = data[1];
        char wkM2 = data[2];
        if(wkM1 != wkM2){
            return;
        }

        radioState.funCod  = 0xF8;
        radioState.workMod = wkM1;

        char ackData[sizeof(VHF_ACK_STATE)];
        memcpy(ackData, &radioState, sizeof(VHF_ACK_STATE));
        RadioManage::getInstance()->onCtrlAck(MessageTyp_VHF_Ack_State, ackData, sizeof(VHF_ASK_STATE));

    }

    if((unsigned char)funCod == 0xFE || (unsigned char)funCod == 0xFE - 0x10){                //输出的设置频道号

        if(len != 3){
            return;
        }

        char chd1 = data[1];
        char chd2 = data[2];
        if(chd1 != chd2){
            return;
        }

        radioState.funCod  = 0xF8;
        radioState.channel = chd1;

        char ackData[sizeof(VHF_ACK_STATE)];
        memcpy(ackData, &radioState, sizeof(VHF_ACK_STATE));
        RadioManage::getInstance()->onCtrlAck(MessageTyp_VHF_Ack_State, ackData, sizeof(VHF_ASK_STATE));

    }
}


int Radio181D::writeCtrlData(uint16_t ctrlTyp, char* data, int len)
{

    QMutexLocker locker(&m_ctrlMutex);
    if(ctrlTyp == MessageTyp_VHF_Set_WorkMod){          //设置工作模式

        int ctrlDataLen = sizeof(VHF_SET_WORKMOD);
        if(len == ctrlDataLen){

            VHF_SET_WORKMOD setWorkMod;
            memcpy(&setWorkMod, data, ctrlDataLen);

            //工作模式 0:语音 1:数传 2:集群
            if(setWorkMod.param1 != 0x00 && setWorkMod.param1 != 0x01 && setWorkMod.param1 != 0x02){
                return 1;
            }

            char srcData[2];
            char dstData[20];
            int  srcLen = 2;
            int  dstLen;

            srcData[0] = 0xF3;
            srcData[1] = setWorkMod.param1;
            if(Protocol == 1){       // 集群协议
                srcData[0] = 0xF3 - 0x10;
            }

            wConverte(srcData, srcLen, dstData, dstLen);
            ctrlCom->write(dstData, dstLen);
        }

    } else if(ctrlTyp == MessageTyp_VHF_Set_Channel){   //设置信道

        int ctrlDataLen = sizeof(VHF_SET_CHANNEL);
        if(len == ctrlDataLen){

            VHF_SET_CHANNEL setChannel;
            memcpy(&setChannel, data, ctrlDataLen);

            char srcData[2];
            char dstData[20];
            int  srcLen = 2;
            int  dstLen;

            srcData[0] = 0xFD;
            srcData[1] = setChannel.param1;
            if(Protocol == 1){       // 集群协议
                srcData[0] = 0xFD - 0x10;
            }

            wConverte(srcData, srcLen, dstData, dstLen);
            ctrlCom->write(dstData, dstLen);
        }

    } else if(ctrlTyp == MessageTyp_VHF_Ask_State){     //状态问询

        int ctrlDataLen = sizeof(VHF_ASK_STATE);
        if(len == ctrlDataLen){

            char srcData[2];
            char dstData[20];
            int  srcLen = 2;
            int  dstLen;

            srcData[0] = 0xF7;
            srcData[1] = 0x01;
            if(Protocol == 1){       // 集群协议
                srcData[0] = 0xF7 - 0x10;
            }

            wConverte(srcData, srcLen, dstData, dstLen);
            ctrlCom->write(dstData, dstLen);
        }

    } else if(ctrlTyp == MessageTyp_VHF_Set_Freq){      //频点设置



    } else if(ctrlTyp == MessageTyp_VHF_Set_Signal){    //



    }

    //            char x[1024];
    //            memset(x, 0, 1024);
    //            memcpy(x, dstData, dstLen);
    //            for (int i = 0; i < dstLen; ++i)
    //                printf("%02x ", x[i]);
    //            printf("------------------------------\n");

    return 0;
}


void Radio181D::wConverte(char* srcData, int srcLen, char* dstData, int &dstLen){

    // eg. 16H 10H 02H + 数据段 + 10H 03H HH LL
    // 数据段写入需要把10H 转为 10H 10H

    int currLen = 0;
    dstData[currLen] = 0x16;

    currLen = currLen + 1;
    dstData[currLen] = 0x10;

    currLen = currLen + 1;
    dstData[currLen] = 0x02;

    for(int i=0; i<srcLen; i++){
        char sData = srcData[i];
        if(sData != 0x10){
            currLen = currLen + 1;
            dstData[currLen] = sData;

        } else {
            currLen = currLen + 1;
            dstData[currLen] = 0x10;
            currLen = currLen + 1;
            dstData[currLen] = 0x10;
        }
    }

    currLen = currLen + 1;
    dstData[currLen] = 0x10;

    currLen = currLen + 1;
    dstData[currLen] = 0x03;

    currLen = currLen + 1;
    dstData[currLen] = 0x00;

    currLen = currLen + 1;
    dstData[currLen] = 0x00;

    dstLen = currLen + 1;

}


int Radio181D::writeLinkData(char* data, int len)
{
    QMutexLocker locker(&m_dataMutex);

    dataCom->write(data, len);
    return 0;
}


void Radio181D::onTimer()
{

//    qDebug()<<"181D onTimer------------------!!!";

    VHF_ASK_STATE askState;
    askState.funCod = 0xF7;
    askState.param1 = 0x01;

    char data[sizeof(VHF_ASK_STATE)];
    memcpy(data, &askState, sizeof(VHF_ASK_STATE));

    writeCtrlData(MessageTyp_VHF_Ask_State, data, sizeof(VHF_ASK_STATE));
}

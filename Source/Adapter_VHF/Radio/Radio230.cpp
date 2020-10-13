#include "Radio230.h"
#include <QDateTime>
#include <QDebug>
#include "log/log4z.h"

using namespace zsummer::log4z;

#define DATA_MAX_SIZE 4096

Radio230::Radio230()
            : updTim(0)
            , m_nModemSend(false)
            , m_nModemState(0)
            , m_SendingTimesCount(0)
            , m_DycArrayLen(0)
            , m_RequestCount(0)
{
    m_DycArrayData = new char[ARRAY_DATA_LEN];
    radioState.errState = 1;

    m_pReceiveBuf = new char[DATA_MAX_SIZE];
    m_dataSndLen = 0;
}

Radio230::~Radio230()
{
    if(dataCom != NULL)
    {
        delete dataCom;
        dataCom = NULL;
    }
    if (NULL != m_DycArrayData)
    {
        delete[] m_DycArrayData;
        m_DycArrayData = NULL;
    }
    if (NULL != m_pReceiveBuf)
    {
        delete[] m_pReceiveBuf;
        m_pReceiveBuf = NULL;
    }
}

void Radio230::serialInit()
{
#if WIN32
    dataCom = new QextSerialPort(QString("COM%1").arg(ConfigLoader::getInstance()->getConfigRadio()->getDataCom()));
#else
    dataCom = new QextSerialPort("/dev/ttymxc2");
#endif
    connect(dataCom, SIGNAL(readyRead()), this, SLOT(readCom()));
    dataCom->setBaudRate(BAUD38400);    //设置波特率
    dataCom->setDataBits(DATA_8);       //设置数据位
    dataCom->setParity(PAR_NONE);       //设置校验
    dataCom->setStopBits(STOP_1);       //设置停止位
    dataCom->setFlowControl(FLOW_OFF);  //设置数据流控制
    dataCom->setTimeout(10);            //设置延时
    //
    if (false == dataCom->open(QIODevice::ReadWrite))
    {
        LOGE("230 Data Serail Open Err!");
    } else {
        LOGI("230 Data Serail Open Success!");
    }

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(1000);

    updTim = QDateTime::currentDateTimeUtc().toTime_t();                     //秒级

    memset(&radioState, 0, sizeof(RADIO_STATE));
}

void Radio230::readCom()
{
    dataArray.push_back(dataCom->readAll());
    packageData();
    parseData();
}

void Radio230::packageData()
{
    if (dataArray.length() < 3)//3 == 包头（1字节）+ 校验（2字节） + 包尾（1字节）
        return;
    else//数据分包
    {
        int  step = 0;
        int  contBegin = 0;
        int  contLen = 0;
        //删除数据长度
        int  removeLen = 0;
        for (int i = 0; i < dataArray.length(); ++i)
        {
            if (step == 0)//定位包头
            {
                if ((unsigned char)dataArray[i] == 0xC0)
                {
                    step = 1;
                    contBegin = i+1;
                    contLen = 0;
                }
            }
            else if (step == 1)//定位包尾
            {
                if ((unsigned char)dataArray[i] == 0xC0)
                {
                    if (contLen == 0)//包头丢失，包尾与下一包包头粘连情况（XX XX C0 C0 XX XX XX XX C0）
                    {
                        contBegin = i+1;
                        contLen = 0;
                        continue;
                    }
                    else if (contLen < 2)//包内数据长度小于校验字节长度2字节，数据长度错误
                    {
                        step = 0;
                        continue;
                    }
                    else
                    {
                        m_recvDataList.push_back(dataArray.mid(contBegin, contLen));
                        //更新已解码成功的数据长度
                        removeLen = contBegin + contLen;
                        step = 0;
                        continue;
                    }
                }
                else
                {
                    ++contLen;
                }
            }

        }
        dataArray.remove(0, removeLen);
    }
}

void Radio230::parseData()
{
    //解析包内容
    while (!m_recvDataList.isEmpty())
    {
        QByteArray tmpArray = m_recvDataList.first();
        m_recvDataList.pop_front();

        char tmp[MAXDATALENGTH];
        memset(tmp, 0, MAXDATALENGTH);
        memcpy(tmp, tmpArray.data(), tmpArray.length());

        char nCRC = getCRC(tmp+5, tmpArray.length()-7);
        char nCRCGet = (tmp[tmpArray.length()-2] << 4) + tmp[tmpArray.length()-1];

        if (nCRC != nCRCGet)
        {
            qDebug() << "In Radio230::recvDataParse, Decoder Recv Data CRC Err!";
        }
        else
        {
            char  state[MAXDATALENGTH];
            memset(state, 0, MAXDATALENGTH);
            int   stateLen = 0;
            decode(tmp, tmpArray.length()-2, state, stateLen);
            //解包到正确数据，判断该数据为数传还是控制信息，分别进行处理
            updateRadioState(state, stateLen);
        }
    }
}

void Radio230::updateRadioState(const char* data, const int len)
{
    radioState.errState = 0;
    updTim = QDateTime::currentDateTimeUtc().toTime_t();                     //秒级

    unsigned char CMDType = (unsigned char)(*(data+4));
    //qDebug() << "###########################" << QString::number(CMDType, 16);

    switch(CMDType)
    {
    case 0x12:		// 2.7.2 电台报告信道信息
    {
        //信道号
        radioState.channel = (*(data+5)<<8) + (*(data+6));

        //发送频率
        uint32_t tmpFreq = 0;
        uint8_t tmp = 0;
        bcd2uint8(*(data+7), &tmp);
        tmpFreq += tmp*1000000;
        bcd2uint8(*(data+8), &tmp);
        tmpFreq += tmp*10000;
        bcd2uint8(*(data+9), &tmp);
        tmpFreq += tmp*100;
        bcd2uint8(*(data+10), &tmp);
        tmpFreq += tmp;
        radioState.txFreq = tmpFreq*10;

        //发射机工作种类
        tmp = *(data+11);
        //调幅话
        if (tmp == 0x31)
            radioState.workTyp = 2;
        //上边带
        else if (tmp == 0x32)
            radioState.workTyp = 0;
        //下边带
        else if (tmp == 0x33)
            radioState.workTyp = 1;
        //等幅报
        else if (tmp == 0x34)
            radioState.workTyp = 3;

        //发射机通信方式（0x30：单工 0x31:双工 0x32:半双工）1字节，未使用
        //发射机功率等级
        tmp = *(data+13);
        //小功率
        if (tmp >= 12)
            radioState.power = 1;
        //中功率
        else if (tmp >= 3)
            radioState.power = 2;
        //高功率
        else
            radioState.power = 0;

        //接收频率
        tmpFreq = 0;
        tmp = 0;
        bcd2uint8(*(data+14), &tmp);
        tmpFreq += tmp*1000000;
        bcd2uint8(*(data+15), &tmp);
        tmpFreq += tmp*10000;
        bcd2uint8(*(data+16), &tmp);
        tmpFreq += tmp*100;
        bcd2uint8(*(data+17), &tmp);
        tmpFreq += tmp;
        radioState.rxFreq = tmpFreq*10;

        //接收机工作种类，1字节，未使用
        //接收机中频带宽，1字节，未使用
        //接收机增益控制，1字节，未使用
    }
        break;
    case 0x23:		// 2.4.4 电台报告业务类型
    {
        if (0x30 == (*(data+5)))
            radioState.workMod = 2;
        else if (0x31 == (*(data+5)))
            radioState.workMod = 1;

        if (0x3f != (*(data+6)))
        {
            //设置电台固定工作在定频模式下
            char pPara[1];
            pPara[0] = 0x03;
            writeData(0x40, 0xa2, pPara, 1);
        }
    }
        break;
    case 0x76:      //状态信息
    {
        if ((unsigned char)*(data+5) == 0x03)
        {
            //信道号
            radioState.channel = (*(data+6)<<8) + (*(data+7));

            //发送频率
            uint32_t tmpFreq = 0;
            uint8_t tmp = 0;
            bcd2uint8(*(data+8), &tmp);
            tmpFreq += tmp*1000000;
            bcd2uint8(*(data+9), &tmp);
            tmpFreq += tmp*10000;
            bcd2uint8(*(data+10), &tmp);
            tmpFreq += tmp*100;
            bcd2uint8(*(data+11), &tmp);
            tmpFreq += tmp;
            radioState.txFreq = tmpFreq*10;

            //发射机工作种类
            tmp = *(data+12);
            //调幅话
            if (tmp == 0x31)
                radioState.workTyp = 2;
            //上边带
            else if (tmp == 0x32)
                radioState.workTyp = 0;
            //下边带
            else if (tmp == 0x33)
                radioState.workTyp = 1;
            //等幅报
            else if (tmp == 0x34)
                radioState.workTyp = 3;

            //发射机功率等级
            tmp = *(data+13);
            //小功率
            if (tmp >= 12)
                radioState.power = 1;
            //中功率
            else if (tmp >= 3)
                radioState.power = 2;
            //高功率
            else
                radioState.power = 0;

            //发射机带宽，1字节，未使用
            //发射机通信方式（0x30：单工 0x31:双工 0x32:半双工）1字节，未使用
            //发射机调谐状态，1字节，未使用

            //接收机信道，2字节，信道只展示发信道，不展示收信道，不使用
            //接收频率
            tmpFreq = 0;
            tmp = 0;
            bcd2uint8(*(data+19), &tmp);
            tmpFreq += tmp*1000000;
            bcd2uint8(*(data+20), &tmp);
            tmpFreq += tmp*10000;
            bcd2uint8(*(data+21), &tmp);
            tmpFreq += tmp*100;
            bcd2uint8(*(data+22), &tmp);
            tmpFreq += tmp;
            radioState.rxFreq = tmpFreq*10;
            //接收机种类，1字节，未使用
            //接收机衰减，1字节，未使用
            //接收机带宽，1字节，未使用
            //接收机增益，3字节，未使用
            //接收机预选器，1字节，未使用
        }
    }
        break;
    case 0x83:		// 2.3 报告数据通信状态
    {
        switch(*(data+5))
        {
        case 0x03:
        {
            m_nModemState	= RADIOMODEMSTATE_RECEIVING;
            qDebug() << "Recving Data";
            memset(m_pReceiveBuf,0,DATA_MAX_SIZE);
            m_dataSndLen = 0;
        }
            break;
        case 0x04:
        {
            m_nModemState	= RADIOMODEMSTATE_RECEIVEEND;
            qDebug() << "Recv Data Done";
            if (m_dataSndLen <= 0)
            {
                break;
            }
            else
            {
                RadioManage::getInstance()->onRecvLinkData(m_pReceiveBuf, m_dataSndLen);
            }
        }
            break;
        case 0x05:
        {
            m_nModemState	= RADIOMODEMSTATE_SENDING;
            qDebug() << "Begin Send Data";
        }
            break;
        case 0x06:
        {
            m_nModemState	= RADIOMODEMSTATE_SENDEND;
            m_nModemSend = false;
            qDebug() << "Send Data Done";
        }
            break;
        case 0x0B:
        {
//            if((RADIOMODEMSTATE_SENDING == m_nModemState ||
//                RADIOMODEMSTATE_BUFEMPTY == m_nModemState) &&
//                true == m_nModemSend)
//            {
//                m_SendingTimesCount++;
//                if (m_SendingTimesCount > 30)
//                {
//                    setModemNoticeSendEnd();
//                    qDebug() << "0x0B m_SendingTimesCount > 30  SetModemNoticeSendEnd!";

//                }
//                else if (m_SendingTimesCount > 40)
//                {
//                    setModemNoticeCancel();
//                    m_SendingTimesCount = 0;
//                    m_nModemSend = false;
//                    m_nModemState = 0;
//                    qDebug() << "0x0B m_SendingTimesCount > 40  SetModemNoticeCancel!";
//                }
//            }
            //m_RadioStateMsgQueryCount = 0;
        }
            break;
        default:
            break;
        }
        // 报告MODEM状态
    }
        break;
    case 0x84:		// 2.3 接收非保密数据
    {
        qDebug() << "Recv Unclassified Data";
        //RadioManage::getInstance()->onRecvLinkData(data+5, len-5);
        memcpy(m_pReceiveBuf+m_dataSndLen, data+5, len-5);
        m_dataSndLen += len-5;
    }
        break;
    case 0x87:		// 2.3 电台发送流控状态
    {
        switch(*(data+5))
        {
        case 0x00:
            m_nModemState	= RADIOMODEMSTATE_BUFFULL;
            radioModemCanSendData(false);
            break;
        case 0x01:
            m_nModemState	= RADIOMODEMSTATE_BUFEMPTY;
            radioModemCanSendData(true);
            m_RequestCount = 0;
            break;
        default:
            break;
        }
    }
        break;
    case 0xA2:		// 2.4 电台报告工作模式
    {
        if (*(data+5) != 0x03)
        {
            //设置电台固定工作在定频模式下
            char pPara[1];
            pPara[0] = 0x03;
            writeData(0x40, 0xa2, pPara, 1);
        }
    }
        break;
    case 0xAB:		// 2.1 握手应答
    {
        if ((unsigned char)*(data+5) == 0x01)
        {
            qDebug() << "Shake Hands Success";
        }
    }
        break;
    case 0xAC:      //NAK
    {
        qDebug() << "Recv NAK";
    }
        break;
   default:
        break;
    }
}

void Radio230::setModemNoticeSendEnd()
{
    // 请求MODEM结束发送数据
    char pPara[1];
    pPara[0] = 0x02;
    writeData(0x40, 0x83, pPara, 1);
}

void Radio230::setModemNoticeCancel()
{
    // 请求MODEM取消发送数据
    char pPara[1];
    pPara[0]	= 0x07;
    writeData(0x40, 0x83, pPara, 1);
}

void Radio230::radioModemCanSendData(bool isSend)
{
    if (true == isSend)
    {
        if (m_DycArrayLen < ARRAY_DATA_LEN)
        {
            int len = 0;
            int copyLen = 0;
            len = (m_DycArrayLen / HFSENDDATA_MAXLIMITLEN) + 1;

            char tempData[HFSENDDATA_LIMITLEN];

            for (int i = 0 ; i < len; i++)
            {
                if (m_nModemState == RADIOMODEMSTATE_SENDEND)
                {
                    //TRACE(_T("超过组织机制分配的时间片\n\r"));
                    m_nModemState = 0;
                    break;
                }
                if (((i+1)* HFSENDDATA_LIMITLEN) < m_DycArrayLen)
                {
                    copyLen = HFSENDDATA_LIMITLEN;
                }
                else
                {
                    copyLen = m_DycArrayLen - (i*HFSENDDATA_LIMITLEN);
                }

                memset(tempData, 0, HFSENDDATA_LIMITLEN);
                memcpy(tempData, m_DycArrayData+(HFSENDDATA_LIMITLEN*i), copyLen);
                writeData(0x41, 0x84, tempData,copyLen);
                LOGD(QString("On radioModemCanSendData copyLen %1").arg(copyLen).toStdString().c_str());
            }

            m_nModemState = RADIOMODEMSTATE_SENDING;

            setModemNoticeSendEnd();
        }
        else
        {
            setModemNoticeSendEnd();
            m_nModemState = 0;
            LOGD(QString("else m_DycArrayLen < ARRAY_DATA_LEN").toStdString().c_str());
        }
    }
}

void Radio230::decode(const char* srcData, const int srcLen, char* dstData, int& dstLen)
{
    for (int m = 0;  m < srcLen; ++m)
    {
        if ((unsigned char)srcData[m] != 0xDB)
        {
            dstData[dstLen] = srcData[m];
            ++dstLen;
        }
        else
        {
            if ((unsigned char)srcData[m+1] == 0xDC)
            {
                dstData[dstLen] = 0xC0;
                ++dstLen;
                ++m;
            }
            else if ((unsigned char)srcData[m+1] == 0xDD)
            {
                dstData[dstLen] = 0xDB;
                ++dstLen;
                ++m;
            }
            else
            {
                dstData[dstLen] = srcData[m];
                ++dstLen;
            }
        }
    }
}

int Radio230::writeCtrlData(uint16_t funCode, char* data, int len)
{
    int ctrlDataLen = sizeof(RADIO_SET);
    if(len == ctrlDataLen)
    {
        RADIO_SET m_set;
        memcpy(&m_set, data, ctrlDataLen);
        switch (funCode)
        {
        case Set_WorkTyp://设置工作方式
        {
            //qDebug() << "Set_WorkTyp";
            setWorkTyp(m_set.workTyp);
        }
            break;
        case Set_WorkMod://设置工作模式
        {
            //qDebug() << "Set_WorkMod";
            setWorkMod(m_set.workMod);
        }
            break;
        case Set_Channel://设置信道
        {
            //qDebug() << "Set_Channel";
            setChannel(m_set.channel);
        }
        break;
        case Set_TxFreq://设置发送频率
        {
            setTxFreq(m_set.txFreq);
        }
            break;
        case Set_RxFreq://设置接收频率
        {
            setRxFreq(m_set.rxFreq);
        }
            break;
        case Set_Power://设置发射功率
        {
            setPower(m_set.power);
        }
            break;
        case Set_Squelch://设置静噪
        {
            setSquelch(m_set.squelch);
        }
            break;
        case Ask_State://状态问询
        {
            char ackData[sizeof(RADIO_STATE)];
            memcpy(ackData, &radioState, sizeof(RADIO_STATE));
            RadioManage::getInstance()->onCtrlAck(Ack_State, ackData, sizeof(RADIO_STATE));
        }
            break;
        default:
            break;
        }
    }
    return 0;
}

int Radio230::writeLinkData(char* data, int len)
{
    QMutexLocker locker(&m_dataMutex);
    //请求发送数据
    char tmp[1];
    tmp[0] = 0x01;
    writeData(0x41, 0x83, tmp, 1);

    if (len <= ARRAY_DATA_LEN)
    {
        memset(m_DycArrayData, 0, ARRAY_DATA_LEN);
        memcpy(m_DycArrayData, data, len);
        m_DycArrayLen = len;
        m_nModemSend = true;
        m_RequestCount++;
        if (m_RequestCount > 4)
        {
            m_RequestCount = 4;
        }
    }
    return 0;
}

void Radio230::writeData(char nDimID, char type, const char* data, const int len)
{
    char tmp[MAXDATALENGTH];
    memset(tmp, 0, MAXDATALENGTH);
    int tmpLen = 0;
    //类型ID
    tmp[0] = 0xA0;
    //信息长度
    tmp[1] = len+7;
    //目的设备ID
    tmp[2] = nDimID;
    //源设备ID
    tmp[3] = 0x42;
    //信令ID
    tmp[4] = type;
    tmpLen += 5;
    //数据
    memcpy(tmp+tmpLen, data, len);
    tmpLen += len;
    char dstData[MAXDATALENGTH];
    memset(dstData, 0, MAXDATALENGTH);
    int dstLen = 0;
    //添加转义
    enCode(tmp, tmpLen, dstData+1, dstLen);
    //添加包头0xC0长度1
    dstLen += 1;
    //CRC校验,只校验参数
    uint16_t t_crc = 0;
    if (len > 0)
        t_crc = getCRC(data, len);

    dstData[dstLen] = t_crc >> 4;
    dstData[dstLen+1] = t_crc & 0x0F;
    dstLen += 2;
    dstData[0] = 0xC0;
    dstData[dstLen] = 0xC0;
    dstLen += 1;
    dataCom->write(dstData, dstLen);
}

void Radio230::enCode(const char* srcData, int srcLen, char* dstData, int& dstLen)
{
    for (int i = 0; i < srcLen; ++i)
    {
        if ((unsigned char)srcData[i] == 0xC0)
        {
            dstData[dstLen] = 0xDB;
            ++dstLen;
            dstData[dstLen] = 0xDC;
            ++dstLen;
        }
        else if ((unsigned char)srcData[i] == 0xDB)
        {
            dstData[dstLen] = 0xDB;
            ++dstLen;
            dstData[dstLen] = 0xDD;
            ++dstLen;
        }
        else
        {
            dstData[dstLen] = srcData[i];
            ++dstLen;
        }
    }
}

char Radio230::getCRC(const char* data, const quint16 len)
{
    if (len <= 0)
        return 0;
    char crc = data[0];
    for (int i = 1; i < len; ++i)
        crc ^= data[i];
    return crc;
}

void Radio230::onTimer()
{
    static uint16_t ShakeHeadCount = 0;
    static uint16_t ChangeToDataModeCount = 0;

    checkDisconnect();

    if (ShakeHeadCount > 120)
    {
        if (m_nModemState != RADIOMODEMSTATE_SENDING)
        {
            ShakeHeadCount = 0;
            //握手
            writeData(0x40, 0xB0, NULL, 0);
        }
    }

    if (ChangeToDataModeCount > 300 && \
        m_nModemState != RADIOMODEMSTATE_SENDING && \
        m_nModemState != RADIOMODEMSTATE_RECEIVING )
    {
        ChangeToDataModeCount = 0;
        char tmp[2];
        tmp[0] = 0x33;
        tmp[1] = 0x03;
        //更改业务类型为数传
        writeData(0x40, 0x23, tmp, 2);
    }

    if(m_RequestCount > 3)
    {
        char nData[1];
        nData[0] = 0x02;
        writeData(0x40, 0x84, nData, 1);
        setModemNoticeSendEnd();
    }

    //查询工作模式
    writeData(0x40, 0x23, NULL, 0);

    ++ShakeHeadCount;
    ++ChangeToDataModeCount;

    //qDebug() << "workTyp: " << QString::number(radioState.workTyp) << ", workMod: " << QString::number(radioState.workMod);
    char ackData[sizeof(RADIO_STATE)];
    memcpy(ackData, &radioState, sizeof(RADIO_STATE));
    RadioManage::getInstance()->onCtrlAck(Ack_State, ackData, sizeof(RADIO_STATE));

}

void Radio230::setWorkTyp(const uint8_t nWorkTyp)
{
    char pPara[2];
    pPara[0]	= 0x03;
    if (nWorkTyp == 0)
        pPara[1]	= 0x32;
    else if (nWorkTyp == 1)
        pPara[1]	= 0x33;
    else if (nWorkTyp == 2)
        pPara[1]	= 0x31;
    else
        pPara[1]	= 0x34;

    //设置接收工作方式
    writeData(0x40, 0x18, pPara, 2);
    //设置发送工作方式
    writeData(0x40, 0x19, pPara, 2);
    //radioState.workTyp = nWorkTyp;
}

void Radio230::setWorkMod(const uint8_t nWorkMod)
{
    char pPara[2];
    if (nWorkMod == 1)
        pPara[0]	= 0x31;
    else
        pPara[0]	= 0x30;
    pPara[1]	= 0x03;

    writeData(0x40, 0x23, pPara, 2);
    //radioState.workMod = nWorkMod;
}

void Radio230::setChannel(const uint16_t nCHN)
{
    char pPara[3];
    pPara[0]	= 0x01;
    pPara[1]	= nCHN/256;
    pPara[2]	= nCHN%256;

    //设置接收信道
    writeData(0x40, 0x18, pPara, 3);
    //设置发送信道
    writeData(0x40, 0x19, pPara, 3);
    //radioState.channel = nCHN;
}

void Radio230::setPower(const uint8_t nPower)
{
    char pPara[2];
    pPara[0]	= 0x04;
    if (nPower == 0)
        pPara[1]	= 0x00;
    else if (nPower == 1)
        pPara[1]	= 0x0C;
    else
        pPara[1]	= 0x03;

    writeData(0x40, 0x19, pPara, 2);
    //radioState.power = nPower;
}

void Radio230::setSquelch(const uint8_t nSquelch)
{
//    char pPara[2];
//    pPara[0]	= 0x02;
//    pPara[1]	= nSquelch;

//    writeData(0x40, 0x22, pPara, 2);
    //radioState.power = nSquelch;
}

void Radio230::setTxFreq(const uint64_t nTxFreq)
{
    //传递设置的数据为khz*10000,转化为hz需除以10
    uint64_t tmp = nTxFreq/10;

    if (tmp < 1600000 || tmp > 30000000)
        return;
    char pPara[5];
    pPara[0]	= 0x02;
    uint82bcd(tmp/1000000, (uint8_t*)(&pPara[1]));
    uint82bcd(tmp/10000%100, (uint8_t*)(&pPara[2]));
    uint82bcd(tmp%10000/100, (uint8_t*)(&pPara[3]));
    uint82bcd(tmp%10000%100, (uint8_t*)(&pPara[4]));

    writeData(0x40, 0x18, pPara, 5);
    radioState.txFreq = nTxFreq;
}

void Radio230::setRxFreq(const uint64_t nRxFreq)
{
    //传递设置的数据为khz*10000,转化为hz需除以10
    uint64_t tmp = nRxFreq/10;

    if (tmp < 1600000 || tmp > 30000000)
        return;
    char pPara[5];
    pPara[0]	= 0x02;
    uint82bcd(tmp/1000000, (uint8_t*)(&pPara[1]));
    uint82bcd(tmp/10000%100, (uint8_t*)(&pPara[2]));
    uint82bcd(tmp%10000/100, (uint8_t*)(&pPara[3]));
    uint82bcd(tmp%10000%100, (uint8_t*)(&pPara[4]));

    writeData(0x40, 0x19, pPara, 5);
    radioState.rxFreq = nRxFreq;
}

void Radio230::checkDisconnect()
{
    long curTim = QDateTime::currentDateTimeUtc().toTime_t();         //秒级
    long difTim = curTim - updTim;

    if(difTim > 5)
    {
        updTim = curTim;
        radioState.errState = 1;       // 电台通信异常
    }
}

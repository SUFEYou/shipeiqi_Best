#include "Radio212tcr.h"
#include <QDateTime>
#include <QDebug>

Radio212TCR::Radio212TCR()
{

}

Radio212TCR::~Radio212TCR()
{

}

void Radio212TCR::serialInit()
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
        qDebug() << "212TCR Data Serail Open Err!";
    } else {
        qDebug() << "212TCR Data Serail Open Success!";
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
        qDebug() << "212TCR Ctrl Serail Open Err!";
    } else {
        qDebug() << "212TCR Ctrl Serail Open Success 100ms!";
    }

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(2000);

    updTim = QDateTime::currentDateTimeUtc().toTime_t();                     //秒级

    memset(&radioState, 0, sizeof(VHF_ACK_STATE));
}


void Radio212TCR::readDataCom()
{
    QByteArray data = dataCom->readAll();
    if(!data.isEmpty())
    {

        //qDebug() << "212TCR DataCom Recv size: " << data.size();
        RadioManage::getInstance()->onRecvLinkData(data);
    }
}



void Radio212TCR::readCtrlCom()
{
    dataArray.push_back(ctrlCom->readAll());
    recvDataSubpackage();
    recvDataParse();
}

void Radio212TCR::recvDataSubpackage()
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

void Radio212TCR::recvDataParse()
{
    //解析包内容
    while (!m_recvDataList.isEmpty())
    {
        QByteArray tmpArray = m_recvDataList.first();
        m_recvDataList.pop_front();

        char tmp[MAXDATALENGTH];
        memset(tmp, 0, MAXDATALENGTH);
        memcpy(tmp, tmpArray.data(), tmpArray.length());

        char nCRC = CRCVerify(tmp, tmpArray.length()-2);
        char nCRCGet = (tmp[tmpArray.length()-2] << 4) + tmp[tmpArray.length()-1];

        if (nCRC != nCRCGet)
        {
            qDebug() << "Decoder Recv TCP Data CRC Err!";
        }
        else
        {
            char  state[MAXDATALENGTH];
            memset(state, 0, MAXDATALENGTH);
            int   stateLen = 0;
            for (int m = 0;  m < tmpArray.length()-2; ++m)
            {
                if ((unsigned char)tmp[m] != 0xDB)
                {
                    state[stateLen] = tmp[m];
                    ++stateLen;
                }
                else
                {
                    if ((unsigned char)tmp[m+1] == 0xDC)
                    {
                        state[stateLen] = 0xC0;
                        ++stateLen;
                        ++m;
                    }
                    else if ((unsigned char)tmp[m+1] == 0xDD)
                    {
                        state[stateLen] = 0xDB;
                        ++stateLen;
                        ++m;
                    }
                    else
                    {
                        state[stateLen] = tmp[m];
                        ++stateLen;
                    }
                }
            }

            //解包到正确数据，发出该数据
            if (stateLen > 0)
                updateRadioState(state, stateLen);
        }
    }
}

char Radio212TCR::CRCVerify(const char* data, const quint16 len)
{
    if (len <= 0)
        return 0;
    char crc = data[0];
    for (int i = 1; i < len; ++i)
        crc ^= data[i];
    return crc;
}


void Radio212TCR::updateRadioState(char* data, int len)
{


}


int Radio212TCR::writeCtrlData(uint16_t ctrlTyp, char* data, int len)
{

    return 0;
}


void Radio212TCR::wConverte(char* srcData, int srcLen, char* dstData, int& dstLen)
{
    // 开始标识
    dstData[dstLen++] = 0XC0;
    //替换0XC0，0XDB
    char tmp[MAXDATALENGTH];
    memset(tmp, 0, MAXDATALENGTH);
    quint16  tmplen = 0;
    for (int i = 0; i < srcLen; ++i)
    {
        if ((unsigned char)srcData[i] == 0xC0)
        {
            tmp[tmplen++]	= 0xDB;
            tmp[tmplen++]	= 0xDC;
        }
        else if ((unsigned char)srcData[i] == 0xDB)
        {
            tmp[tmplen++]	= 0xDB;
            tmp[tmplen++]	= 0xDD;
        }
        else
        {
            tmp[tmplen++]	= srcData[i];
        }
    }
    //数据域
    memcpy(dstData+dstLen, tmp, tmplen);
    dstLen += tmplen;
    unsigned char nCRCData = CRCVerify(dstData+1, dstLen-1);
    //CRC校验
    dstData[dstLen++] = nCRCData >> 4;
    dstData[dstLen++] = nCRCData & 0X0F;
    //结束标识
    dstData[dstLen++] = 0XC0;
}


int Radio212TCR::writeLinkData(char* data, int len)
{
    QMutexLocker locker(&m_dataMutex);

    dataCom->write(data, len);
    return 0;
}


void Radio212TCR::onTimer()
{


}

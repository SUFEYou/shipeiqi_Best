#include "Radio171al.h"
#include <QDebug>

Radio171AL::Radio171AL()
{

}

Radio171AL::~Radio171AL()
{

}

void Radio171AL::serialInit()
{
#if WIN32
    dataCom = new QextSerialPort("COM1");
#else
    dataCom = new QextSerialPort("/dev/ttymxc1");
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
        qDebug() << "171al Serail Open Err!";
    } else {
        qDebug() << "171al Serail Open Success!";
    }
}


void Radio171AL::readCom()
{
    dataArray.push_back(dataCom->readAll());
    recvDataSubpackage();
}


void Radio171AL::updateRadioState(char* data, int len)
{


}


int Radio171AL::writeCtrlData(uint16_t ctrlTyp, char* data, int len)
{


}

void Radio171AL::wConverte(char* srcData, int srcLen, char* dstData, int &dstLen){



}


int Radio171AL::writeLinkData(char* data, int len)
{


}


void Radio171AL::onTimer()
{


}

void Radio171AL::recvDataSubpackage()
{
    if (dataArray.length() < 8)//8 == 包头(1字节)+类型ID(2字节)+信息长度(2字节)+校验(2字节)+包尾(1字节)
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
                    else if (contLen < 6)//包内数据长度小于8字节，数据长度错误
                    {
                        step = 0;
                        continue;
                    }
                    else
                    {
                        m_recvDataList.push_back(dataArray.mid(contBegin, contLen));
                        //更新已解码成功的数据长度
                        //removeLen = contBegin + contLen + 1;
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

void Radio171AL::recvDataParse()
{
    while (!m_recvDataList.isEmpty())
    {
        QByteArray tmpArray = m_recvDataList.first();
        m_recvDataList.pop_front();

        char tmp[4096];
        int len = tmpArray.length();
        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, tmpArray.data(), len);

        uint16_t t_crc = getCRC(tmp, len-2);
        uint16_t recv_crc = (tmp[len-2]<<8) | (tmp[len-1]);
        if ( t_crc != recv_crc)
        {
            qDebug() << "In Radio171AL::recvDataParse(), CRC Err";
            return;
        }
        char dstData[4096];
        memset(dstData, 0, sizeof(dstData));
        int dstLen = 0;
        rConverte(tmp, len, dstData, dstLen);
        int messageLen = (dstData[2]<<8) | (dstData[3]);
        //dstLen-6 == 数据域长度 == dstLen - 类型ID(2字节) - 信息长度(2字节) - 校验(2字节)
        if (messageLen != (dstLen-6))
        {
            qDebug() << "In Radio171AL::recvDataParse(), Message Len Err";
            return;
        }
    }
}

void Radio171AL::rConverte(const char* srcData, const int srcLen, char* dstData, int &dstLen)
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

uint16_t Radio171AL::getCRC(unsigned char* buf, unsigned int len)
{
    unsigned int i, j;
    uint16_t crc, flag;
    bool escapeCharacter = false;
    crc = 0x0000;

    for (i = 0; i < len; i++)
    {
        if ((buf[i] == 0xDB) && !escapeCharacter)
        {
            escapeCharacter = true;
            continue;
        }
        else
        {
            if ((buf[i] == 0xDC) && escapeCharacter)
            {
                buf[i] = 0xC0;
                escapeCharacter =false;
            }
            else if ((buf[i] == 0xDD) && escapeCharacter)
            {
                buf[i] = 0xDB;
                escapeCharacter =false;
            }
            else if (escapeCharacter)
            {
                return 0;
            }
        }
        crc ^= ((static_cast<unsigned short>(buf[i]))<<8);
        for (j = 0; j < 8; j++)
        {
            flag = crc&0x8000;
            crc <<= 1;
            if (flag)
            {
                crc &= 0xfffe;
                crc ^= 0x8005;
            }
        }
    }
    return crc;
}

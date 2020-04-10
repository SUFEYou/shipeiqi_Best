#include "Radio171d.h"
#include <QDebug>

Radio171D::Radio171D()
{

}

Radio171D::~Radio171D()
{

}

void Radio171D::serialInit()
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
        qDebug() << "171d Serail Open Err!";
    } else {
        qDebug() << "171d Serail Open Success!";
    }
}


void Radio171D::readCom()
{

}



void Radio171D::updateRadioState(char* data, int len)
{


}


int Radio171D::writeCtrlData(uint16_t ctrlTyp, char* data, int len)
{


}


void Radio171D::wConverte(char* srcData, int srcLen, char* dstData, int &dstLen){



}


int Radio171D::writeLinkData(char* data, int len)
{


}


void Radio171D::onTimer()
{


}

uint16_t Radio171D::getCRC(unsigned char* buf, unsigned int len)
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


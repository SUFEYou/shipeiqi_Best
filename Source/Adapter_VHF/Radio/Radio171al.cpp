#include "Radio171al.h"
#include <qdebug.h>

Radio171AL::Radio171AL()
{

}

Radio171AL::~Radio171AL()
{

}

void Radio171AL::serialInit()
{
#if WIN32
    com = new QextSerialPort("COM1");
#else
    com = new QextSerialPort("/dev/ttymxc1");
#endif
    connect(com, SIGNAL(readyRead()), this, SLOT(readCom()));
    com->setBaudRate(BAUD38400);    //设置波特率
    com->setDataBits(DATA_8);       //设置数据位
    com->setParity(PAR_NONE);       //设置校验
    com->setStopBits(STOP_1);       //设置停止位
    com->setFlowControl(FLOW_OFF);  //设置数据流控制
    com->setTimeout(10);            //设置延时
    //
    if (false == com->open(QIODevice::ReadWrite))
    {
        qDebug() << "Radio-171AL Com Open Err!";
    } else {
        qDebug() << "Radio-171AL Com Open Success!";
    }

}

void Radio171AL::readCom()
{
    QByteArray data = com->readAll();
    if(!data.isEmpty())
    {

        qDebug() << "181D DataCom Recv size: " << data.size();
        //emit comRecData(temp);
    }
}


int Radio171AL::writeCtrlData(uint16_t ctrlTyp, char* data, int len)
{

    return 0;
}

int Radio171AL::writeLinkData(char* data, int len)
{

    return 0;
}

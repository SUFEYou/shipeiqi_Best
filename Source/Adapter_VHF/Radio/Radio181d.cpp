#include "Radio181d.h"
#include "RadioManage.h"
#include <stdint.h>
#include <QDebug>

Radio181D::Radio181D()
{

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
    ctrlCom = new QextSerialPort("/dev/ttymxc3");
#endif

    connect(ctrlCom, SIGNAL(readyRead()), this, SLOT(readCtrlCom()));
    ctrlCom->setBaudRate(BAUD19200);    //设置波特率
    ctrlCom->setDataBits(DATA_8);       //设置数据位
    ctrlCom->setParity(PAR_NONE);       //设置校验
    ctrlCom->setStopBits(STOP_1);       //设置停止位
    ctrlCom->setFlowControl(FLOW_OFF);  //设置数据流控制
    ctrlCom->setTimeout(10);            //设置延时
    //
    if (false == dataCom->open(QIODevice::ReadWrite))
    {
        qDebug() << "181D Ctrl Serail Open Err!";
    } else {
        qDebug() << "181D Ctrl Serail Open Success!";
    }

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
    QByteArray data = ctrlCom->readAll();
    if(!data.isEmpty())
    {
        qDebug() << "181D CtrlCom Recv size: " << data.size();

    }
}

int Radio181D::writeCtrlData(char* data, int len)
{
    if(len < 8){
        return -1;
    }
    QMutexLocker locker(&m_ctrlMutex);

    uint8_t sByte     = data[0];
    uint8_t radioStd  = data[1];           //

    uint16_t radioID;
    memcpy(radioID, data[2], 2);

    uint8_t ctrType   = data[4];
    uint32_t setByte;

    if(ctrType == Channel_Set){
        setByte = data[5];
    }

    if(ctrType == Work_Mod_Set){
        setByte = data[5];
    }


    return 0;
}

int Radio181D::writeLinkData(char* data, int len)
{
    QMutexLocker locker(&m_dataMutex);

    dataCom->write(data, len);
    return 0;
}

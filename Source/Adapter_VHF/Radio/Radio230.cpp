#include "Radio230.h"
#include <QDebug>
#include <QDateTime>

Radio230::Radio230()
{
    memset(&radioState, 0, sizeof(RADIO_STATE));
    radioState.errState = 1;
}

Radio230::~Radio230()
{
    if(ctrlCom != NULL){
        delete ctrlCom;
        ctrlCom = NULL;
    }
    if(dataCom != NULL){
        delete dataCom;
        dataCom = NULL;
    }
}

void Radio230::serialInit()
{
#if WIN32
    dataCom = new QextSerialPort("COM10");
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
        qDebug() << "230 Data Serail Open Err!";
    } else {
        qDebug() << "230 Data Serail Open Success!";
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
        qDebug() << "230 Ctrl Serail Open Err!";
    } else {
        qDebug() << "230 Ctrl Serail Open Success 100ms!";
    }

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(2000);

    updTim = QDateTime::currentDateTimeUtc().toTime_t();                     //秒级
}

int Radio230::writeCtrlData(uint16_t funCode, char* data, int len)
{

}

int Radio230::writeLinkData(char* data, int len)
{
    QMutexLocker locker(&m_dataMutex);

    dataCom->write(data, len);
    return 0;
}

void Radio230::readDataCom()
{
    QByteArray data = dataCom->readAll();
    if(!data.isEmpty())
    {
        //qDebug() << "230 DataCom Recv size: " << data.size();
        RadioManage::getInstance()->onRecvLinkData(data);
    }
}

void Radio230::readCtrlCom()
{
    dataArray.push_back(ctrlCom->readAll());
    packageData();
}

void Radio230::packageData()
{
    if (dataArray.length() < 3)//3 == 设备地址码（1字节）+ 包头02H（1字节） + 包尾03H（1字节）
        return;
    else//数据分包
    {
        int  step = 0;
        int  contBegin = 0;
        int  contLen = 0;
        //删除数据长度
        int  removeLen = 0;
        unsigned char deviceAddr = 0;
        for (int i = 0; i < dataArray.length(); ++i)
        {
            if (step == 0)//定位包头
            {
                if ((unsigned char)dataArray[i] == 0x02)
                {
                    if (i > 0)
                    {
                        deviceAddr = (unsigned char)dataArray[i-1];
                        step = 1;
                        contBegin = i+1;
                        contLen = 0;
                    }
                }
            }
            else if (step == 1)//定位包尾
            {
                if ((unsigned char)dataArray[i] == 0x03)
                {
                    if (contLen < 2)//包内数据长度小于校验字节长度2字节，数据长度错误
                    {
                        step = 0;
                        deviceAddr = 0;
                        continue;
                    }
                    else
                    {
                        parseData(deviceAddr, dataArray.mid(contBegin, contLen));
                        //更新已解码成功的数据长度
                        removeLen = contBegin + contLen;
                        step = 0;
                        deviceAddr = 0;
                        continue;
                    }
                }
                else if ((unsigned char)dataArray[i] == 0x02)
                {
                    step = 1;
                    contBegin = i+1;
                    contLen = 0;
                    deviceAddr = (unsigned char)dataArray[i-1];
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

void Radio230::parseData(const unsigned char nDeviceAddr, const QByteArray array)
{
    //一帧数据
    if (array.length() <= 5)
    {

    }
    else//多帧数据
    {

    }
}

void Radio230::onTimer()
{

}

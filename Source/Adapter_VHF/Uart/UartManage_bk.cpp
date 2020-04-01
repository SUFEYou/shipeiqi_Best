#include "UartManage_bk.h"
#include <QDebug>
#include "Uart/qextserialport.h"

UartManage* UartManage::m_instance = NULL;
QMutex UartManage::m_mutex;

UartManage::UartManage(QObject *parent)
           : QObject(parent)
{

}

UartManage::~UartManage()
{

}

UartManage* UartManage::getInstance()
{

    QMutexLocker locker(&m_mutex);
    if (m_instance == NULL)
    {
        m_instance = new UartManage;
    }

    return m_instance;
}

void UartManage::init()
{
    serialInit();
}

void UartManage::serialInit()
{
#if WIN32
    m_myCom = new QextSerialPort("COM1");
#else
    m_myCom = new QextSerialPort("/dev/ttymxc1");
#endif
    connect(m_myCom, SIGNAL(readyRead()), this, SLOT(readMyCom()));
    //设置波特率
    m_myCom->setBaudRate(BAUD38400);
    //设置数据位
    m_myCom->setDataBits(DATA_8);
    //设置校验
    m_myCom->setParity(PAR_NONE);
    //设置停止位
    m_myCom->setStopBits(STOP_1);
    //设置数据流控制
    m_myCom->setFlowControl(FLOW_OFF);
    //设置延时
    m_myCom->setTimeout(10);
    //
    if (false == m_myCom->open(QIODevice::ReadWrite))
    {
        qDebug() << "Serial Open Err!";
    }
    else
    {
        qDebug() << "Serial Open Success!";
    }
}

void UartManage::readMyCom()
{
    //static quint64 recvsize = 0;
    QByteArray temp = m_myCom->readAll();
    if(!temp.isEmpty())
    {
//        recvsize += temp.size();
        qDebug() << "Com Recv size: " << temp.size();
//        //发送数据
//        m_myCom->write(temp);
        emit comRecData(temp);
    }
}

void UartManage::comSendData(char* pChar,int nLen)
{
    qDebug() << "Com Send size: " << nLen;
    m_myCom->write(pChar, nLen);
}

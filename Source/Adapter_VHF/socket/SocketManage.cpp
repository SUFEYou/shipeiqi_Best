#include "SocketManage.h"
#include <QTcpSocket>
#include <QUdpSocket>
#include "config/ConfigLoader.h"
#include "socket/TCPDataDeal.h"
#include <QDebug>
#include <QTimer>

SocketManage* SocketManage::m_socketMannage = NULL;
QMutex  SocketManage::m_instanceMutex;

SocketManage::SocketManage(QObject *parent)
             : QObject(parent)
             , m_tcpSocket(new QTcpSocket(this))
             , m_udpSocket(new QUdpSocket(this))
             , m_timer(new QTimer(this))
             , m_bACCLinkOk(false)
{
    m_nACCCloseCt		= 0;			// 断链计数
    m_nACCCloseCtLmt	= 30;			// 断链计数限制
    m_nACCUpdateCt		= 0;			// 上报状态计数
    m_nACCUpdateCtLmt	= 60;			// 上报状态计数限制

    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(tcpConnected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(tcpDisconnected()));
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(tcpError(QAbstractSocket::SocketError)));
    connect(m_tcpSocket,SIGNAL(readyRead()), this, SLOT(tcpReadData()));
    connect(m_udpSocket,SIGNAL(readyRead()), this, SLOT(udpReadData()));

    connect(m_timer, SIGNAL(timeout()), this, SLOT(dealTimer()));
    m_timer->start(1000);
}

SocketManage::~SocketManage()
{
    if (m_udpSocket != NULL)
        delete m_udpSocket;
    if (m_tcpSocket != NULL)
        delete m_tcpSocket;
    if (m_socketMannage != NULL)
        delete m_socketMannage;
}

SocketManage* SocketManage::getInstance()
{
    if (m_socketMannage == NULL)
    {
        QMutexLocker locker(&m_instanceMutex);
        if (m_socketMannage == NULL)
        {
            m_socketMannage = new SocketManage;
        }
    }
    return m_socketMannage;
}

void SocketManage::dealTimer()
{
    if (m_bACCLinkOk)
    {
        m_nACCCloseCt	= 0;
        m_nACCUpdateCt++;
        if (m_nACCUpdateCt > m_nACCUpdateCtLmt)
        {
            m_nACCUpdateCt = 0;
            TCPDataDeal::getInstance()->RSCtoACCUpdateStateInfo();
        }

    }
    else
    {
        m_nACCCloseCt++;
        if (m_nACCCloseCt >= m_nACCCloseCtLmt)
        {
            m_tcpSocket->abort();
            m_tcpSocket->connectToHost(QHostAddress(m_communicateIP), m_dataTransPort);
        }
    }
}

void SocketManage::init()
{
    m_communicateIP = ConfigLoader::getInstance()->getCommunicateIP();
    m_dataTransPort = ConfigLoader::getInstance()->getDataTransPort();
    m_ctlRevPort = ConfigLoader::getInstance()->getCtlRevPort();
    m_ctlSndPort = ConfigLoader::getInstance()->getCtlSndPort();
    m_tcpSocket->abort();
    m_tcpSocket->connectToHost(QHostAddress(m_communicateIP), m_dataTransPort);
    m_udpSocket->bind(QHostAddress::Any, m_ctlRevPort, QUdpSocket::ShareAddress);
}

void SocketManage::tcpConnected()
{
    qDebug() << "TCP Connected";
    m_bACCLinkOk = true;
}

void SocketManage::tcpDisconnected()
{
    qDebug() << "TCP Disconnected";
    m_bACCLinkOk = false;
}

void SocketManage::tcpError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qDebug() << "TCP Err: " << m_tcpSocket->errorString();
    m_bACCLinkOk = false;
}

void SocketManage::tcpReadData()
{
    QByteArray array;
    array = m_tcpSocket->readAll();
    if (array.length() > 0)
    {
        TCPDataDeal::getInstance()->recvTCPData(array.data(), array.length());
    }
}

void SocketManage::tcpSendData(unsigned char* pData,int nLen)
{
    m_tcpSocket->write((char*)(pData), nLen);
}

void SocketManage::udpReadData()
{
    while (m_udpSocket->hasPendingDatagrams())
    {
       QByteArray datagram;
       datagram.resize(m_udpSocket->pendingDatagramSize());
       m_udpSocket->readDatagram(datagram.data(), datagram.size());
       qDebug() << " Recv UDP Data :" << datagram.size();
    }
}

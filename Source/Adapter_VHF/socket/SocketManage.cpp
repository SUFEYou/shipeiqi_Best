#include "SocketManage.h"
#include <QTcpSocket>
#include <QUdpSocket>
#include "config/ConfigLoader.h"
#include "socket/TCPDataDecoder.h"
#include <QDebug>

SocketManage* SocketManage::m_socketMannage = NULL;
QMutex  SocketManage::m_instanceMutex;

SocketManage::SocketManage(QObject *parent)
             : QObject(parent)
             , m_tcpSocket(new QTcpSocket(this))
             , m_udpSocket(new QUdpSocket(this))
{
    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(tcpConnected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(tcpDisconnected()));
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(tcpError(QAbstractSocket::SocketError)));
    connect(m_tcpSocket,SIGNAL(readyRead()), this, SLOT(tcpReadData()));
    connect(m_udpSocket,SIGNAL(readyRead()), this, SLOT(udpReadData()));
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
}

void SocketManage::tcpDisconnected()
{
    qDebug() << "TCP Disconnected";
}

void SocketManage::tcpError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qDebug() << "TCP Err: " << m_tcpSocket->errorString();
}

void SocketManage::tcpReadData()
{
    QByteArray array;
    array = m_tcpSocket->readAll();
    if (array.length() > 0)
    {
        TCPDataDecoder::getInstance()->recvTCPData(array.data(), array.length());
    }
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
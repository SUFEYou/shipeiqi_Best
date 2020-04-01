#include "TcpClient.h"
#include <QTcpSocket>
#include <QUdpSocket>
#include "config/ConfigLoader.h"
#include "TCPDataDeal.h"
#include <QDebug>
#include <QTimer>


TcpClient::TcpClient()
{

}


void TcpClient::init()
{
    m_nACCCloseCt		= 0;			// 断链计数
    m_nACCCloseCtLmt	= 30;			// 断链计数限制
    m_nACCUpdateCt		= 0;			// 上报状态计数
    m_nACCUpdateCtLmt	= 60;			// 上报状态计数限制

    QString serIP = ConfigLoader::getInstance()->getTcpIP();
    int serPort   = ConfigLoader::getInstance()->getTcpPort();

    m_tcpSocket = new QTcpSocket(this);
    m_tcpSocket->abort();
    m_tcpSocket->connectToHost(QHostAddress(serIP), serPort);

    m_timer = new QTimer(this);

    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(tcpConnected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(tcpDisconnected()));
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(tcpError(QAbstractSocket::SocketError)));
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(tcpReadData()));

    connect(m_timer, SIGNAL(timeout()), this, SLOT(dealTimer()));
    m_timer->start(1000);

}

void TcpClient::sendData(unsigned char* pData,int nLen)
{
    if(m_bACCLinkOk){
        m_tcpSocket->write((char*)(pData), nLen);
    }

}

void TcpClient::tcpConnected()
{
    qDebug() << "TCP Connected";
    m_bACCLinkOk = true;
}

void TcpClient::tcpDisconnected()
{
    qDebug() << "TCP Disconnected";
    m_bACCLinkOk = false;
}

void TcpClient::tcpError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qDebug() << "TCP Err: " << m_tcpSocket->errorString();
    m_bACCLinkOk = false;
}

void TcpClient::tcpReadData()
{
    QByteArray array;
    array = m_tcpSocket->readAll();
    if (array.length() > 0)
    {
        TCPDataDeal::getInstance()->recvTCPData(array.data(), array.length());
    }
}

void TcpClient::dealTimer()
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
            QString serIP = ConfigLoader::getInstance()->getTcpIP();
            int serPort   = ConfigLoader::getInstance()->getTcpPort();
            m_tcpSocket->abort();
            m_tcpSocket->connectToHost(QHostAddress(serIP), serPort);
        }
    }
}

#include "UdpRctrl.h"
#include <QTime>
#include <QDebug>
#include "config/ConfigLoader.h"
#include "Socket/SocketManage.h"
#include "Radio/RadioManage.h"

UDPRctrl::UDPRctrl()
{

}

void UDPRctrl::init(int recivPort, QString sndToIP, int sndToPort)
{
    this->m_sndToIP   = sndToIP;
    this->m_recivPort = recivPort;
    this->m_sndToPort = sndToPort;

    m_udpSocket = new QUdpSocket(this);

    connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(onRev()));
    connect(m_udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    m_udpSocket->bind(QHostAddress::Any, m_recivPort, QUdpSocket::ShareAddress);

}


void UDPRctrl::onRev()
{
    while (m_udpSocket->hasPendingDatagrams())
    {
       QByteArray datagram;
       datagram.resize(m_udpSocket->pendingDatagramSize());
       m_udpSocket->readDatagram(datagram.data(), datagram.size());

       char* data = datagram.data();
       int nLen   = datagram.size();

       RadioManage::getInstance()->writeCtrlData(data, nLen);
    }
}


void UDPRctrl::sendData(char* pData,int nLen)
{
    m_udpSocket->writeDatagram(pData, nLen, m_sndToIP, m_sndToPort);
}


void UDPRctrl::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qDebug() << m_udpSocket->errorString();
}

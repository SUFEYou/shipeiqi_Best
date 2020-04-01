#include "UdpVoice.h"
#include <QDebug>
#include <QTime>
#include <QWaitCondition>
#include "Socket/SocketManage.h"
#include "config/ConfigLoader.h"

QMutex wmutex;
quint8 FrameSN = -1;

UDPVoice::UDPVoice()
{

}


void UDPVoice::init(int recivPort, QString sndToIP, int sndToPort)
{
    this->m_sndToIP   = sndToIP;
    this->m_recivPort = recivPort;
    this->m_sndToPort = sndToPort;

    m_udpSocket = new QUdpSocket(this);

    connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(onRev()));
    connect(m_udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    m_udpSocket->bind(QHostAddress::Any, m_recivPort, QUdpSocket::ShareAddress);

}

void UDPVoice::onRev()
{
    while (m_udpSocket->hasPendingDatagrams())
    {
       QByteArray datagram;
       datagram.resize(m_udpSocket->pendingDatagramSize());
       m_udpSocket->readDatagram(datagram.data(), datagram.size());

       char* data = datagram.data();
       int nLen   = datagram.size();
       int nCurLen= 0;

//       qDebug() << QTime::currentTime().msec() << " Recv UDP Data :" << datagram.size();


    }
}

void UDPVoice::sendData(char* pData,int nLen)
{
    m_udpSocket->writeDatagram(pData, nLen, m_sndToIP, m_sndToPort);
}


void UDPVoice::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qDebug() << m_udpSocket->errorString();
}

#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QUdpSocket>
#include <QAbstractSocket>

class UDPVoice: public QObject
{
    Q_OBJECT
public:
    UDPVoice();
    void init(int recivPort, QString sndToIP, int sndToPort);
    void sendData(char* pData,int nLen);

private:

private slots:
    void onRev();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QUdpSocket          *m_udpSocket;
    QHostAddress        m_sndToIP;
    quint16             m_recivPort;
    quint16             m_sndToPort;
    quint8              m_id;

};

#endif // UDPSOCKET_H

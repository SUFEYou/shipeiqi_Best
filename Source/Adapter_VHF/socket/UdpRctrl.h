#ifndef UDPRCTL_H
#define UDPRCTL_H

#include <QUdpSocket>
#include <QAbstractSocket>

class UDPRctrl: public QObject
{
    Q_OBJECT
public:
    UDPRctrl();
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

#endif // UDPRCTL_H

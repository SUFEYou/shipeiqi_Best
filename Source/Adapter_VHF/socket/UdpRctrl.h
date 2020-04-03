#ifndef UDPRCTL_H
#define UDPRCTL_H

#include <QUdpSocket>
#include <QAbstractSocket>
#include <QMutex>
#include <common.h>
#include <socket/socketcommon.h>

class UDPRctrl: public QObject
{
    Q_OBJECT
public:
    UDPRctrl();
    void init(int recivPort, QString sndToIP, int sndToPort);
    void sendData(char* pData,int nLen);
    void sendCtrlAck(uint16_t ackTyp, char* pData,int nLen);

private:
    void registPipe(REGIST_PIPE pipe);

private slots:
    void onRev();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QUdpSocket          *m_udpSocket;
    QHostAddress        m_sndToIP;
    quint16             m_recivPort;
    quint16             m_sndToPort;
    quint8              m_id;


    QList<REGIST_PIPE>   regPipeList;

};

#endif // UDPRCTL_H

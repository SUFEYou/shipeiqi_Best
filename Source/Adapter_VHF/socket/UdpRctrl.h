#ifndef UDPRCTL_H
#define UDPRCTL_H

#include <QUdpSocket>
#include <QAbstractSocket>
#include <QMutex>
#include <RadioLink/LinkCommon.h>
#include <socket/socketcommon.h>

class UDPRctrl: public QObject
{
    Q_OBJECT
public:
    UDPRctrl();
    void init(int port);
    void sendData(char* pData,int nLen);
    void sendCtrlAck(uint16_t ackTyp, char* pData,int nLen);

private:
    void registCtrl(CTRL_REGIST_VO pipe);

private slots:
    void onRev();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QUdpSocket          *m_udpSocket;
    quint16             m_Port;

    QMutex               regMutex;
    QList<CTRL_REGIST_VO>   regList;

};

#endif // UDPRCTL_H

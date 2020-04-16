#ifndef UDPRCTL_H
#define UDPRCTL_H

#include <QUdpSocket>
#include <QAbstractSocket>
#include <QMutex>
#include <socket/SocketCommu.h>

class UDPRctrl: public QObject
{
    Q_OBJECT
public:
    UDPRctrl();
    void init(int port);
    void sendData(char* pData,int nLen);
    void sendCtrlAck(uint16_t ackTyp, char* pData,int nLen);

private:
    void registCtrl(QString sessionKey, CTRL_REGIST_VO pipe);
    int  getRegistedID(QString sessionKey);
    void sendRegistState(uint8_t regState, QHostAddress netAddr, uint32_t netPort);

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

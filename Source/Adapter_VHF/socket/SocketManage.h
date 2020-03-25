#ifndef SOCKETMANAGE_H
#define SOCKETMANAGE_H

#include <QObject>
#include <QMutex>
#include <QAbstractSocket>

class QTcpSocket;
class QUdpSocket;

class SocketManage : public QObject
{
    Q_OBJECT
public:
    static SocketManage* getInstance();
    void init();


private:
    SocketManage(QObject *parent = NULL);
    ~SocketManage();

private slots:
    void tcpConnected();
    void tcpDisconnected();
    void tcpReadData();
    void tcpError(QAbstractSocket::SocketError socketError);
    void udpReadData();


private:
    static SocketManage                 *m_socketMannage;
    static QMutex                       m_instanceMutex;

    QTcpSocket                          *m_tcpSocket;
    QUdpSocket                          *m_udpSocket;

    QString                             communicateIP;
    int                                 dataTransPort;
    int                                 ctlRevPort;
    int                                 ctlSndPort;
};

#endif // SOCKETMANAGE_H

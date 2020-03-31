#ifndef SOCKETMANAGE_H
#define SOCKETMANAGE_H

#include <QObject>
#include <QMutex>
#include <QAbstractSocket>

class QTcpSocket;
class QUdpSocket;
class QTimer;

class SocketManage : public QObject
{
    Q_OBJECT
public:
    static SocketManage* getInstance();
    void init();
    void tcpSendData(unsigned char* pData = NULL,int nLen = 0);

private:
    SocketManage(QObject *parent = NULL);
    ~SocketManage();

private slots:
    void tcpConnected();
    void tcpDisconnected();
    void tcpReadData();
    void tcpError(QAbstractSocket::SocketError socketError);
    void udpReadData();

    void dealTimer();

private:
    static SocketManage                 *m_socketMannage;
    static QMutex                       m_instanceMutex;

    QTcpSocket                          *m_tcpSocket;
    QUdpSocket                          *m_udpSocket;

    QString                             m_communicateIP;
    int                                 m_dataTransPort;
    int                                 m_ctlRevPort;
    int                                 m_ctlSndPort;

    QTimer                              *m_timer;
    bool                                m_bACCLinkOk;
    int                                 m_nACCCloseCt;			// 断链计数
    int                                 m_nACCCloseCtLmt;		// 断链计数限制
    int                                 m_nACCUpdateCt;			// 上报状态计数
    int                                 m_nACCUpdateCtLmt;		// 上报状态计数限制
};

#endif // SOCKETMANAGE_H

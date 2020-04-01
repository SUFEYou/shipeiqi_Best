#ifndef TCPDATA_H
#define TCPDATA_H

#include <QObject>
#include <QTcpSocket>
#include <QMutex>
#include <QAbstractSocket>
#include <qtimer.h>

class TcpClient: public QObject
{
    Q_OBJECT
public:
    TcpClient();
    void init();

    void sendData(unsigned char* pData = NULL,int nLen = 0);

private slots:
    void tcpConnected();
    void tcpDisconnected();
    void tcpReadData();
    void tcpError(QAbstractSocket::SocketError socketError);
    void dealTimer();

private:
    QTcpSocket                          *m_tcpSocket;
    QTimer                              *m_timer;

    bool                                m_bACCLinkOk;
    int                                 m_nACCCloseCt;			// 断链计数
    int                                 m_nACCCloseCtLmt;		// 断链计数限制
    int                                 m_nACCUpdateCt;			// 上报状态计数
    int                                 m_nACCUpdateCtLmt;		// 上报状态计数限制
};

#endif // TCPDATA_H

#ifndef SOCKETMANAGE_H
#define SOCKETMANAGE_H

#include <QObject>
#include <QMutex>
#include "TcpClient.h"
#include "UdpRctrl.h"
#include "UdpVoice.h"

class QTcpSocket;
class QUdpSocket;
class QTimer;

class SocketManage : public QObject
{
    Q_OBJECT
public:
    static SocketManage* getInstance();
    void init();

    inline TcpClient *getTcpClient() const { return tcpClient; }
    inline UDPRctrl  *getCtrlUdp()   const { return rctrlUdp;  }
    inline UDPVoice  *getVoicUdp()   const { return voiceUdp;  }

private:
    SocketManage();
    ~SocketManage();


private:
    static SocketManage           *m_socketMannage;
    static QMutex                 m_instanceMutex;

    TcpClient                     *tcpClient;
    UDPRctrl                      *rctrlUdp;
    UDPVoice                      *voiceUdp;

};

#endif // SOCKETMANAGE_H

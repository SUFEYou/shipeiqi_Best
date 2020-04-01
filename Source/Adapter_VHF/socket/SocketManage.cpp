#include "SocketManage.h"
#include <QTcpSocket>
#include <QUdpSocket>
#include <QDebug>
#include "TcpClient.h"
#include "config/ConfigLoader.h"

SocketManage* SocketManage::m_socketMannage = NULL;
QMutex  SocketManage::m_instanceMutex;

SocketManage::SocketManage()
{

}

SocketManage::~SocketManage()
{

}

SocketManage* SocketManage::getInstance()
{
    if (m_socketMannage == NULL)
    {
        QMutexLocker locker(&m_instanceMutex);
        if (m_socketMannage == NULL)
        {
            m_socketMannage = new SocketManage;
        }
    }
    return m_socketMannage;
}


void SocketManage::init()
{

    QString sndToIP = ConfigLoader::getInstance()->getCtrlSndToIP();
    int vSndToPort  = ConfigLoader::getInstance()->getVoicSndToPort();
    int vRecivPort  = ConfigLoader::getInstance()->getVoicOnRevPort();
    int cSndToPort  = ConfigLoader::getInstance()->getCtrlSndToPort();
    int cRecivPort  = ConfigLoader::getInstance()->getCtrlOnRevPort();

    voiceUdp = new UDPVoice();
    voiceUdp->init(vRecivPort, sndToIP, vSndToPort);

    rctrlUdp = new UDPRctrl();
    rctrlUdp->init(cRecivPort, sndToIP, cSndToPort);

    tcpClient = new TcpClient();
    tcpClient->init();
}


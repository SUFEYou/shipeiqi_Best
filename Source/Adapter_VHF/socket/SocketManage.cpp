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

    int ctrlPort   = ConfigLoader::getInstance()->getCtrlPort();
    int voicPort   = ConfigLoader::getInstance()->getVoicPort();

    voiceUdp = new UDPVoice();
    voiceUdp->init(voicPort);

    rctrlUdp = new UDPRctrl();
    rctrlUdp->init(ctrlPort);

    tcpClient = new TcpClient();
    tcpClient->init();
}


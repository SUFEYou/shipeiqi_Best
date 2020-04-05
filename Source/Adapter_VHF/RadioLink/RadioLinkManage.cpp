#include "RadioLinkManage.h"
#include "RadioLinkMaster.h"
#include "RadioLinkClient.h"
#include "Radio/RadioManage.h"
#include <QDebug>

RadioLinkManage* RadioLinkManage::m_instance = NULL;
QMutex RadioLinkManage::m_mutex;

RadioLinkManage::RadioLinkManage()
                : m_radioLinkMaster(new RadioLinkMaster)
                , m_radioLinkClient(new RadioLinkClient)
{

}

RadioLinkManage::~RadioLinkManage()
{
    if (m_radioLinkMaster != NULL)
    {
        delete m_radioLinkMaster;
        m_radioLinkMaster = NULL;
    }
    if (m_radioLinkClient != NULL)
    {
        delete m_radioLinkClient;
        m_radioLinkClient = NULL;
    }
}

RadioLinkManage* RadioLinkManage::getInstance()
{
    if (m_instance == NULL)
    {
        QMutexLocker locker(&m_mutex);
        if (m_instance == NULL)
        {
            m_instance = new RadioLinkManage;
        }
    }
    return m_instance;
}

void RadioLinkManage::OnCommRecData(const QByteArray &data)
{
    if (m_radioLinkClient->GetAvailable())
    {
        qDebug() << "m_pLayerVHFClient UseDataInput nLen: " << data.length();
        m_radioLinkClient->recvData(data.data(), data.length());
    }
    else if (m_radioLinkMaster->GetAvailable())
    {
        qDebug() << "m_pLayerVHFHead UseDataInput nLen: " << data.length();;
        m_radioLinkMaster->recvData(data.data(), data.length());
    }
}

bool RadioLinkManage::PortCommSendOutData(char* pChar,int nLen)
{
    RadioManage::getInstance()->writeLinkData(pChar, nLen);
    return true;
}

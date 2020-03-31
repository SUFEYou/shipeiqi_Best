#include "CE_VHFNodeManage.h"
#include "VHFLayer/CSC_01LayerHead.h"
#include "VHFLayer/CSC_01LayerClient.h"
#include "Uart/UartManage.h"
#include <QTimer>
#include <QDebug>

CE_VHFNodeManage* CE_VHFNodeManage::m_instance = NULL;
QMutex CE_VHFNodeManage::m_mutex;

CE_VHFNodeManage::CE_VHFNodeManage()
                 : m_pLayerVHFHead(new CSC_01LayerHead)
                 , m_pLayerVHFClient(new CSC_01LayerClient)
                 , m_VHFLayerTimer(new QTimer(this))
{
    m_pLayerVHFClient->UseSetTerminalTag(CTerminalBase::TERM_VHFLAYER);
    m_pLayerVHFHead->UseSetTerminalTag(CTerminalBase::TERM_VHFLAYER);

    connect(UartManage::getInstance(), SIGNAL(comRecData(QByteArray)), this, SLOT(OnCommRecData(QByteArray)));
    connect(this, SIGNAL(comSendData(char*,int)), UartManage::getInstance(), SLOT(comSendData(char*,int)));

    connect(m_VHFLayerTimer, SIGNAL(timeout()), this, SLOT(dealVHFLayerTimer()));
}

CE_VHFNodeManage::~CE_VHFNodeManage()
{
    if (m_pLayerVHFClient != NULL)
        delete m_pLayerVHFClient;
    if (m_pLayerVHFHead != NULL)
        delete m_pLayerVHFHead;

}

CE_VHFNodeManage* CE_VHFNodeManage::getInstance()
{
    if (m_instance == NULL)
    {
        QMutexLocker locker(&m_mutex);
        if (m_instance == NULL)
        {
            m_instance = new CE_VHFNodeManage;
        }
    }
    return m_instance;
}

void CE_VHFNodeManage::init()
{
    //////////////////////////////////////////////////////////////////////////
        // 链路基本信息
    m_pLayerVHFHead->m_nCodeMe = 1000;
    m_pLayerVHFClient->m_nCodeMe = 1000;

    m_pLayerVHFClient->m_nDataMaxLen = 1020;
    m_pLayerVHFHead->m_nDataMaxLen = 1020;

    m_pLayerVHFClient->m_nTimeFactor = 5;
    m_pLayerVHFHead->m_nTimeFactor = 5;

    m_pLayerVHFClient->m_nTCircleDrift = 1;
    m_pLayerVHFHead->m_nTCircleDrift = 1;

    m_pLayerVHFClient->SetAvailable(true);
    m_pLayerVHFClient->m_bMonitorAll	= true;

    m_pLayerVHFHead->SetAvailable(false);
    m_pLayerVHFHead->m_bMonitorAll	= true;

    m_VHFLayerTimer->start(200);
}

void CE_VHFNodeManage::OnCommRecData(const QByteArray &data)
{
    if (m_pLayerVHFClient->GetAvailable())
    {
        qDebug() << "m_pLayerVHFClient UseDataInput nLen: " << data.length();
        m_pLayerVHFClient->UseDataInput(data.data(), data.length());
    }
    else if (m_pLayerVHFHead->GetAvailable())
    {
        qDebug() << "m_pLayerVHFHead UseDataInput nLen: " << data.length();;
        m_pLayerVHFHead->UseDataInput(data.data(), data.length());
    }
}

bool CE_VHFNodeManage::PortCommSendOutData(char* pChar,int nLen)
{
    emit comSendData(pChar, nLen);
    return true;
}

void CE_VHFNodeManage::dealVHFLayerTimer()
{
    if (m_pLayerVHFHead->GetAvailable())
    {
        m_pLayerVHFHead->XTimeMaintenanceCircle();
    }
    if (m_pLayerVHFClient->GetAvailable())
    {
        m_pLayerVHFClient->XTimeMaintenanceCircle();
    }
}

void CE_VHFNodeManage::VHFLayerChangeClientToHead()
{
    // Take Message that Can Change Client to Head
    if (m_pLayerVHFClient->GetAvailable())
    {
        // Close the VHF Layer Client
        m_pLayerVHFClient->SetAvailable(false);
        m_pLayerVHFClient->LinkLayerCircleMomentToDrift();



        if (m_pLayerVHFHead->m_nChain.nListMember.length() == 0)
        {
            // Add MySelf into Layer
            pCSCObjStage obj(new CSCObjStage);
            obj->id = m_pLayerVHFHead->m_nCodeMe;
            obj->degree	= 1;
            obj->state	= CSC_01Layer::LAYSTA_ONLINE;
            obj->signal	= CSC_01Layer::LAYSTA_SUCCESS;

            m_pLayerVHFHead->m_nChain.nListMember.push_back(obj);
            m_pLayerVHFHead->m_nChain.nMemNum = 1;
        }

        // Chain Information
        m_pLayerVHFHead->m_nChain.nChainId = int(m_pLayerVHFHead->m_nCodeMe/1000);

        // Initialize the VHF Layer Head Information
        m_pLayerVHFHead->SetAvailable(true);
        m_pLayerVHFHead->LinkLayerCircleMomentToBegin();
    }
}

#include "RadioLinkManage.h"
#include "RadioLinkMaster.h"
#include "RadioLinkClient.h"
#include "Radio/RadioManage.h"
#include "socket/TCPDataProcess.h"
#include <time.h>
#include <QDebug>

RadioLinkManage* RadioLinkManage::m_instance = NULL;
QMutex RadioLinkManage::m_mutex;

RadioLinkManage::RadioLinkManage()
                : m_radioLinkMaster(new RadioLinkMaster)
                , m_radioLinkClient(new RadioLinkClient)
                , m_timer(new QTimer(this))
{
    m_pBufSend		= new char[RADIORTCCMDLEN];		// 发送数据缓存
    m_nBufSendLen	= 0;	// 发送数据缓存长度
    m_pExDataSend   = new char[RADIORTCCMDLEN];		// 发送数据缓存
    m_nExDataSLen	= 0;	// 发送数据缓存长度

    memset(&m_sSendHead,0,sizeof(NET_MSG_HEADER));
    m_sSendHead.MessageModel	= A01VLN_MSGMODEL_STATION_EX;
    m_nIDMe		= 0;

    m_MRTPosData = new char[256];			//rodar of data
    m_MRTPosDataLen = 0;
    memset(m_MRTPosData,0,256);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(dealTimer()));
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
    if (m_pBufSend != NULL)
    {
        delete[] m_pBufSend;
        m_pBufSend = NULL;
    }
    if (m_pExDataSend != NULL)
    {
        delete[] m_pExDataSend;
        m_pExDataSend = NULL;
    }
    if (m_MRTPosData != NULL)
    {
        delete[] m_MRTPosData;
        m_MRTPosData = NULL;
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

void RadioLinkManage::init()
{
    //////////////////////////////////////////////////////////////////////////
        // 链路基本信息
    m_sSendHead.ProgramType	= 13;
    m_sSendHead.ProgramID	= 7997;

    m_nIDMe = 11997;
    m_radioLinkClient->setCodeMe(m_nIDMe);
    m_radioLinkMaster->setCodeMe(m_nIDMe);

    m_radioLinkClient->setDataMaxLen(1020);
    m_radioLinkMaster->setDataMaxLen(1020);

    m_radioLinkClient->setTimeFactor(5);
    m_radioLinkMaster->setTimeFactor(5);

    m_radioLinkClient->setCircleDrift(1);
    m_radioLinkMaster->setCircleDrift(1);

    m_radioLinkClient->SetAvailable(true);
    m_radioLinkClient->setMonitorAll(true);

    m_radioLinkMaster->SetAvailable(false);
    m_radioLinkMaster->setMonitorAll(true);

    m_timer->start(200);
}

RadioLink* RadioLinkManage::GetLayer()
{
    if (m_radioLinkMaster->GetAvailable())
    {
        return m_radioLinkMaster;
    }
    else if (m_radioLinkClient->GetAvailable())
    {
        return m_radioLinkClient;
    }
    else
    {
        return NULL;
    }
}

void RadioLinkManage::changeClientToMaster()
{
    // Take Message that Can Change Client to Head
    if (m_radioLinkClient->GetAvailable())
    {
        // Close the VHF Layer Client
        m_radioLinkClient->SetAvailable(false);
        m_radioLinkClient->LinkLayerCircleMomentToDrift();


        ModelChain& tmpModelChain = m_radioLinkMaster->getModelChain();
        if (tmpModelChain.nListMember.length() == 0)
        {
            // Add MySelf into Layer
            pObjStage obj(new ObjStage);
            obj->id = m_radioLinkMaster->getCodeMe();
            obj->degree	= 1;
            obj->state	= RadioLink::LAYSTA_ONLINE;
            obj->signal	= RadioLink::LAYSTA_SUCCESS;

            tmpModelChain.nListMember.push_back(obj);
            tmpModelChain.nMemNum = 1;
        }

        // Chain Information
        tmpModelChain.nChainId = int(m_radioLinkMaster->getCodeMe()/1000);

        // Initialize the VHF Layer Head Information
        m_radioLinkMaster->SetAvailable(true);
        m_radioLinkMaster->LinkLayerCircleMomentToBegin();
    }
}

//////////////////////////////////////////////////////////////////////////
// 信息交互操作
//////////////////////////////////////////////////////////////////////////
// ==>> 向下发送

// 发送报文
void RadioLinkManage::ACCtoRSCMessageData(const int nSendID, const int nRecvID, char* pChar,const int nLen, bool bEncrypt,int nDegree, const int nSerial)
{
    pVHFMsg Newmsg(new ObjMsg);
    Newmsg->nSource		= nSendID;
    Newmsg->nReceive	= nRecvID;
    Newmsg->nDataLen	= nLen;
    memcpy(Newmsg->pData,pChar,nLen);
    Newmsg->nDegree		= nDegree;
    Newmsg->bEncrypt		= bEncrypt;
    Newmsg->nSerial		= nSerial;
    Newmsg->nTimeCount	= 0;
    Newmsg->nSendtimes	= 0;

    //qDebug() << "Recv ACC Message, Message Serial: " << Newmsg->nSerial;

    bool bInsert = false;
    for (int i = 0; i < m_lMsgList.length(); ++i)
    {
        if (m_lMsgList[i]->nDegree > Newmsg->nDegree)
        {
            m_lMsgList.insert(i, Newmsg);
            bInsert = true;
            break;
        }
    }
    if (!bInsert)
    {
        m_lMsgList.push_back(Newmsg);
    }
}

//移动台位置报文
void RadioLinkManage::ACCtoRSCPosData(const char* pchar, const int len)
{
    memcpy(m_MRTPosData, pchar, len);
    m_MRTPosDataLen = len;
}

//删除报文
bool RadioLinkManage::DeleteACCtoRSCMessageData(const int nSendID, const int nSerialBegin, const int nSerialEnd)
{
    QMutexLocker locker(&m_listMutex);
    bool bFinder = false;

    QList<pVHFMsg>::iterator iter = m_lMsgList.begin();
    while (iter != m_lMsgList.end())
    {
        if (((*iter)->nSource == nSendID) && \
            ((*iter)->nSerial >= nSerialBegin) && \
            ((*iter)->nSerial <= nSerialEnd) )
        {
            bFinder = true;
            if ((*iter)->nSerial == nSerialEnd)
            {
                m_lMsgList.erase(iter);
                break;
            }
            else
            {
                iter = m_lMsgList.erase(iter);
            }
        }
        else
        {
            ++iter;
        }
    }
    return bFinder;
}

// 向上反馈 ==>>
// 上报当前接收到的报文
void RadioLinkManage::PackToSendRMTtoRSCMessageData(const int nSendID,const int nRecvID,char* pChar,const int nLen,bool bEncrypt)
{
    // 向ACC汇报当前所到的报文
    memset(m_pBufSend,0,RADIORTCCMDLEN);
    m_nBufSendLen	= sizeof(NET_MSG_HEADER);

    NET_MSGEX_TEXT sText;
    sText.SendID	= nSendID;
    sText.RecvID	= nRecvID;
    sText.Degree	= 0;
    sText.Encrypt	= bEncrypt;
    sText.TextLength= nLen;
    sText.Serial	= 0;

    memcpy(m_pBufSend+m_nBufSendLen,&sText,sizeof(NET_MSGEX_TEXT));
    m_nBufSendLen	+= sizeof(NET_MSGEX_TEXT);

    memcpy(m_pBufSend+m_nBufSendLen-1,pChar,nLen);
    m_nBufSendLen	+= (nLen-1);

    // Header Information
    time_t ltime;
    time(&ltime);
    m_sSendHead.MessageLen		= m_nBufSendLen;
    m_sSendHead.MessageSerial	= (unsigned long)(ltime);
    m_sSendHead.MessageType		= VLNMSG_MSGEX_TEXT;
    memcpy(m_pBufSend,&m_sSendHead,sizeof(NET_MSG_HEADER));

    TCPDataProcess::getInstance()->packageAndSendData(m_pBufSend, m_nBufSendLen);

}

// 上报用户报文发送完成
void RadioLinkManage::PackToSendRMTtoRSCMessageSerial(const int nSendID,const int nSerial)
{
    if (nSendID > 0)
    {
        // 生成报文发送给ACC
        memset(m_pBufSend,0,RADIORTCCMDLEN);
        m_nBufSendLen	= sizeof(NET_MSG_HEADER);

        NET_MSGEX_RECALLCODE sRecall;
        sRecall.SendID	= nSendID;
        sRecall.Serial	= nSerial;

        memcpy(m_pBufSend+m_nBufSendLen,&sRecall,sizeof(NET_MSGEX_RECALLCODE));
        m_nBufSendLen	+= sizeof(NET_MSGEX_RECALLCODE);

        // Header Information
        time_t ltime;
        time(&ltime);
        m_sSendHead.MessageLen		= m_nBufSendLen;
        m_sSendHead.MessageSerial	= (unsigned long)(ltime);
        m_sSendHead.MessageType		= VLNMSG_MSGEX_RECALLCODE;
        memcpy(m_pBufSend,&m_sSendHead,sizeof(NET_MSG_HEADER));

        TCPDataProcess::getInstance()->packageAndSendData(m_pBufSend, m_nBufSendLen);
    }
}

// 上报当前链表状态
void RadioLinkManage::RSCtoACCChainState()
{
    ModelChain cChain;
    if (m_radioLinkMaster->GetAvailable())
    {
        cChain	= m_radioLinkMaster->getModelChain();
    }
    else if (m_radioLinkClient->GetAvailable())
    {
        cChain	= m_radioLinkClient->getModelChain();
    }

    // 生成报文发送给ACC
    memset(m_pBufSend,0,RADIORTCCMDLEN);
    m_nBufSendLen	= sizeof(NET_MSG_HEADER);

    NET_RSC_UPDATEMRT sChain;
    sChain.RSCID	=TCPDataProcess::getInstance()->getRSCID();
    sChain.Count	= (unsigned short)(cChain.nListMember.length());
    memcpy(m_pBufSend+m_nBufSendLen, &sChain, sizeof(NET_RSC_UPDATEMRT));
    m_nBufSendLen	+= sizeof(NET_RSC_UPDATEMRT);

    time_t ltime;
    time(&ltime);

    for (int i = 0; i < cChain.nListMember.length(); ++i)
    {
        NET_MRT_STATE sState;
        sState.inRSCID	=  sChain.RSCID;
        sState.MRTID	= cChain.nListMember[i]->id;
        sState.LastTime	= (unsigned long)(ltime);
        sState.SignalLevel	= cChain.nListMember[i]->signal;
        sState.State	= cChain.nListMember[i]->state;
        sState.Going	= cChain.nListMember[i]->sequence;

        memcpy(m_pBufSend+m_nBufSendLen-sizeof(NET_MRT_STATE), &sState, sizeof(NET_MRT_STATE));
        m_nBufSendLen	+= sizeof(NET_MRT_STATE);
    }

    // Header Information
    m_sSendHead.MessageLen		= m_nBufSendLen;
    m_sSendHead.MessageSerial	= (unsigned long)(ltime);
    m_sSendHead.MessageType		= VLNMSG_RSC_UPDATEMRT;
    memcpy(m_pBufSend,&m_sSendHead,sizeof(NET_MSG_HEADER));

    TCPDataProcess::getInstance()->packageAndSendData(m_pBufSend, m_nBufSendLen);
}

// <==接收到链锯回馈的处理
bool  RadioLinkManage::RMTtoRSCMessageSerial(const int nSendID,const int nSerial)
{
    //qDebug() << "RMTtoRSCMessageSerial SendID " << nSendID << " nSerial " << nSerial;

    QMutexLocker locker(&m_listMutex);
    bool bFinder = false;
    for (int i = 0; i < m_lMsgList.length(); ++i)
    {
        if ((m_lMsgList[i]->nSource == nSendID) && (m_lMsgList[i]->nSerial == nSerial))
        {
            //qDebug() << "RMTtoRSCMessageSerial RemoveAt SendID " << nSendID << ", nSerial " << nSerial;
            m_lMsgList.removeAt(i);
            bFinder = true;

            PackToSendRMTtoRSCMessageSerial(nSendID,nSerial);
            break;
        }
    }

    return bFinder;
}

void  RadioLinkManage::ReSetListCountNum()
{

}


void RadioLinkManage::OnCommRecData(const QByteArray &data)
{
    if (m_radioLinkClient->GetAvailable())
    {
        m_radioLinkClient->recvData(data.data(), data.length());
    }
    else if (m_radioLinkMaster->GetAvailable())
    {
        m_radioLinkMaster->recvData(data.data(), data.length());
    }
}

bool RadioLinkManage::PortCommSendOutData(char* pChar,int nLen)
{
    RadioManage::getInstance()->writeLinkData(pChar, nLen);
    return true;
}

// No.4 Condition
// Get the Layer Message And Change the State
////////////////////////////////////////////////////
// Get the Waiting Send Message
// Return Value:
//		  -2:Not Find the Receive ID
//		  -1:Pointer Not Exist
//		   0:Have Been Out the Length Limit
//         1:Still Have Space to Add the Data
//         2:No Data Need be Sended
int RadioLinkManage::sendDataFromListWait(int maxlength)
{
    if (m_MRTPosDataLen > 0)
    {
        setMRTPosDataAppend(m_MRTPosData,m_MRTPosDataLen,m_nIDMe,BROADCAST_ID,-1,maxlength);
        m_MRTPosDataLen = 0;
        memset(m_MRTPosData,0,256);
    }


    bool bFind = false;
    QMutexLocker locker(&m_listMutex);
    for (int a = 0; a < m_lMsgList.length(); ++a)
    {
        ObjMsg& msg = *(m_lMsgList[a].data());
        int i = setDataAppend(msg, maxlength);

        //qDebug() << "VHFLayerSendDataFromListWait msg.nSource " << msg.nSource << ", msg.nSerial " << msg.nSerial;

        if (i > 0) // Still Can Add Message Again
        {
            // Add Data Success
            if (bFind == false)
            {
                bFind = true;
            }
            continue;
        }
        else
        {
            return i;
        }
    }

    if (bFind == false)
    {
        return 2L;		// Not data Need to Send
    }
    else
    {
        return 1L;		// Have Still Space
    }

    return 0L;
}

// Append Message Data to Send Data
// Return Value:
//		  -2:Not Find the Receive ID
//		  -1:Pointer Not Exist
//		   0:Have Been Out the Length Limit
//         1:Still Have Space to Add the Data
int RadioLinkManage::setDataAppend(ObjMsg& stumsg, int maxlength)
{
    RadioLink* m_pLayer = GetLayer();
    if (!m_pLayer)
    {
        return -1L;
    }

    // Judge the Length of the VHF Layer Memory Space Leave
    int t = m_pLayer->DataLayerSendMemoryGatherJudge(false,stumsg.nDataLen,maxlength);

    if ( t == 1)
    {
        memset(m_pExDataSend,0,RADIORTCCMDLEN);
        m_nExDataSLen = 0;

        m_pLayer->onceMessageEncode(stumsg.pData,stumsg.nDataLen,
                                             false,stumsg.nSource,stumsg.nReceive,m_pExDataSend,m_nExDataSLen,stumsg.nSerial);

        m_pLayer->DataLayerSendMemoryGather((char*)m_pExDataSend,m_nExDataSLen);
        stumsg.nSendtimes++;
    }
    else
    {
        qDebug() << "In CE_VHFNodeManage::XSetVHFDataAppend() t != 1";
    }

    return t;
}

int RadioLinkManage::setMRTPosDataAppend(char* pPosData,int PosDataLen,int sendId,int recvID,int serial,int maxlength)
{

    RadioLink* m_pLayer = GetLayer();
    if (!m_pLayer)
    {
        return -1L;
    }

    // Judge the Length of the VHF Layer Memory Space Leave
    int t = m_pLayer->DataLayerSendMemoryGatherJudge(false,PosDataLen,maxlength);

    if ( t == 1)
    {
        memset(m_pExDataSend,0,RADIORTCCMDLEN);
        m_nExDataSLen = 0;

        m_pLayer->onceMessageEncode(pPosData,PosDataLen,false,sendId,recvID,m_pExDataSend,m_nExDataSLen,serial);

        m_pLayer->DataLayerSendMemoryGather(m_pExDataSend,m_nExDataSLen);
    }

    return t;
}

void RadioLinkManage::dealTimer()
{
    if (m_radioLinkMaster->GetAvailable())
    {
        m_radioLinkMaster->timerProcess();
    }
    if (m_radioLinkClient->GetAvailable())
    {
        m_radioLinkClient->timerProcess();
    }
}

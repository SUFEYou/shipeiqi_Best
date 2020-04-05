#include "CE_VHFNodeManage.h"
#include "VHFLayer/CSC_01LayerHead.h"
#include "VHFLayer/CSC_01LayerClient.h"
#include "socket/TCPDataProcess.h"
#include <time.h>
#include <QTimer>
#include <QDebug>
#include <Radio/RadioManage.h>

CE_VHFNodeManage* CE_VHFNodeManage::m_instance = NULL;
QMutex CE_VHFNodeManage::m_mutex;

CE_VHFNodeManage::CE_VHFNodeManage()
                 : m_pLayerVHFHead(new CSC_01LayerHead)
                 , m_pLayerVHFClient(new CSC_01LayerClient)
                 , m_VHFLayerTimer(new QTimer(this))
{
    m_pLayerVHFClient->UseSetTerminalTag(CTerminalBase::TERM_VHFLAYER);
    m_pLayerVHFHead->UseSetTerminalTag(CTerminalBase::TERM_VHFLAYER);

    m_pBufSend		= new char[RADIORTCCMDLEN];		// 发送数据缓存
    m_nBufSendLen	= 0;	// 发送数据缓存长度
    m_pExDataSend   = new char[RADIORTCCMDLEN];		// 发送数据缓存
    m_nExDataSLen	= 0;	// 发送数据缓存长度

    memset(&m_sSendHead,0,sizeof(NET_MSG_HEADER));
    m_sSendHead.MessageModel	= A01VLN_MSGMODEL_STATION_EX;
    m_nVHFIDMe		= 0;

    m_MRTPosData = new char[256];			//rodar of data
    m_MRTPosDataLen = 0;
    memset(m_MRTPosData,0,256);

    connect(m_VHFLayerTimer, SIGNAL(timeout()), this, SLOT(dealVHFLayerTimer()));
}

CE_VHFNodeManage::~CE_VHFNodeManage()
{
    if (m_pLayerVHFClient != NULL)
    {
        delete m_pLayerVHFClient;
        m_pLayerVHFClient = NULL;
    }
    if (m_pLayerVHFHead != NULL)
    {
        delete m_pLayerVHFHead;
        m_pLayerVHFHead = NULL;
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
    m_sSendHead.ProgramType	= 13;
    m_sSendHead.ProgramID	= 7997;

    m_pLayerVHFHead->m_nCodeMe = 11997;
    m_pLayerVHFClient->m_nCodeMe = 11997;

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
//    emit comSendData(pChar, nLen);
    RadioManage::getInstance()->writeLinkData(pChar, nLen);
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

// VHF Layer Control
CSC_01Layer* CE_VHFNodeManage::GetLayerVHF()
{
    if (m_pLayerVHFHead->GetAvailable())
    {
        return m_pLayerVHFHead;
    }
    else if (m_pLayerVHFClient->GetAvailable())
    {
        return m_pLayerVHFClient;
    }
    else
    {
        return NULL;
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

//////////////////////////////////////////////////////////////////////////
// 信息交互操作
//////////////////////////////////////////////////////////////////////////

void CE_VHFNodeManage::ACCtoRSCMessageData(const int nSendID, const int nRecvID, char* pChar,const int nLen, bool bEncrypt,int nDegree, const int nSerial)
{
    pVHFMsg Newmsg(new CSCRSC_ObjVHFMsg);
    Newmsg->nSource		= nSendID;
    Newmsg->nReceive	= nRecvID;
    Newmsg->nDataLen	= nLen;
    memcpy(Newmsg->pData,pChar,nLen);
    Newmsg->nDegree		= nDegree;
    Newmsg->bEncrypt		= bEncrypt;
    Newmsg->nSerial		= nSerial;
    Newmsg->nTimeCount	= 0;
    Newmsg->nSendtimes	= 0;

    qDebug() << "Recv ACC Message, Message Serial: " << Newmsg->nSerial;

    bool bInsert = false;
    for (int i = 0; i < m_lVHFMsgList.length(); ++i)
    {
        if (m_lVHFMsgList[i]->nDegree > Newmsg->nDegree)
        {
            m_lVHFMsgList.insert(i, Newmsg);
            bInsert = true;
            break;
        }
    }
    if (!bInsert)
    {
        m_lVHFMsgList.push_back(Newmsg);
    }
}


// 向上反馈 ==>>
// 上报当前接收到的报文
void CE_VHFNodeManage::PackToSendRMTtoRSCMessageData(const int nSendID,const int nRecvID,char* pChar,const int nLen,bool bEncrypt)
{
    // 向ACC汇报当前所到的报文
    memset(m_pBufSend,0,RADIORTCCMDLEN);
    m_nBufSendLen	= sizeof(NET_MSG_HEADER);

    NET_MSGEX_TEXT sText;
    sText.SendID	= nSendID;
    sText.RecvID	= nRecvID;
    sText.Degree	= 0;
    sText.Encrypt	= bEncrypt;
    sText.TextLength	= nLen;
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
void CE_VHFNodeManage::PackToSendRMTtoRSCMessageSerial(const int nSendID,const int nSerial)
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
void CE_VHFNodeManage::RSCtoACCChainState()
{
    CSCModelChain cChain;
    if (m_pLayerVHFHead->GetAvailable())
    {
        cChain	= m_pLayerVHFHead->m_nChain;
    }
    else if (m_pLayerVHFClient->GetAvailable())
    {
        cChain	= m_pLayerVHFClient->m_nChain;
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
bool  CE_VHFNodeManage::RMTtoRSCMessageSerial(const int nSendID,const int nSerial)
{
    qDebug() << "RMTtoRSCMessageSerial SendID " << nSendID << " nSerial " << nSerial;

    QMutexLocker locker(&m_listMutex);
    bool bFinder = false;
    for (int i = 0; i < m_lVHFMsgList.length(); ++i)
    {
        if ((m_lVHFMsgList[i]->nSource == nSendID) && (m_lVHFMsgList[i]->nSerial == nSerial))
        {
            qDebug() << "RMTtoRSCMessageSerial RemoveAt SendID " << nSendID << ", nSerial " << nSerial;
            m_lVHFMsgList.removeAt(i);
            bFinder = true;

            PackToSendRMTtoRSCMessageSerial(nSendID,nSerial);
            break;
        }
    }

    return bFinder;
}

// No.4 Condition
// Get the Layer Message And Change the State
////////////////////////////////////////////////////
// VHF Layer Get the Waiting Send Message
// Return Value:
//		  -2:Not Find the Receive ID
//		  -1:Pointer Not Exist
//		   0:Have Been Out the Length Limit
//         1:Still Have Space to Add the Data
//         2:No Data Need be Sended
int CE_VHFNodeManage::VHFLayerSendDataFromListWait(int maxlength)
{
    if (m_MRTPosDataLen > 0)
    {
        XSetVHFMRTPosDataAppend(m_MRTPosData,m_MRTPosDataLen,m_nVHFIDMe,BROADCAST_ID,-1,maxlength);
        m_MRTPosDataLen = 0;
        memset(m_MRTPosData,0,256);
    }


    bool bFind = false;
    QMutexLocker locker(&m_listMutex);
    for (int a = 0; a < m_lVHFMsgList.length(); ++a)
    {
        CSCRSC_ObjVHFMsg& msg = *(m_lVHFMsgList[a].data());
        int i = XSetVHFDataAppend(msg, maxlength);

        qDebug() << "VHFLayerSendDataFromListWait msg.nSource " << msg.nSource << ", msg.nSerial " << msg.nSerial;

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

// Append Message Data to VHF Send Data
// Return Value:
//		  -2:Not Find the Receive ID
//		  -1:Pointer Not Exist
//		   0:Have Been Out the Length Limit
//         1:Still Have Space to Add the Data
int CE_VHFNodeManage::XSetVHFDataAppend(CSCRSC_ObjVHFMsg& stumsg,int maxlength)
{
    CSC_01Layer* m_pVHFLayer = GetLayerVHF();
    if (!m_pVHFLayer)
    {
        return -1L;
    }

    // Judge the Length of the VHF Layer Memory Space Leave
    int t = m_pVHFLayer->DataLayerSendMemoryGatherJudge(false,stumsg.nDataLen,maxlength);

    if ( t == 1)
    {
        memset(m_pExDataSend,0,RADIORTCCMDLEN);
        m_nExDataSLen = 0;

        m_pVHFLayer->DataLayerMessageOnceEncode(stumsg.pData,stumsg.nDataLen,
                                                false,stumsg.nSource,stumsg.nReceive,(char*)m_pExDataSend,m_nExDataSLen,stumsg.nSerial);

        m_pVHFLayer->DataLayerSendMemoryGather((char*)m_pExDataSend,m_nExDataSLen);
        stumsg.nSendtimes++;
    }
    else
    {
        qDebug() << "In CE_VHFNodeManage::XSetVHFDataAppend() t != 1";
    }

    return t;
}




int CE_VHFNodeManage::XSetVHFMRTPosDataAppend(char* pPosData,int PosDataLen,int sendId,int recvID,int serial,int maxlength)
{

    CSC_01Layer* m_pVHFLayer = GetLayerVHF();
    if (!m_pVHFLayer)
    {
        return -1L;
    }

    // Judge the Length of the VHF Layer Memory Space Leave
    int t = m_pVHFLayer->DataLayerSendMemoryGatherJudge(false,PosDataLen,maxlength);

    if ( t == 1)
    {
        memset(m_pExDataSend,0,RADIORTCCMDLEN);
        m_nExDataSLen = 0;

        m_pVHFLayer->DataLayerMessageOnceEncode((char*)pPosData,PosDataLen,false,sendId,recvID,(char*)m_pExDataSend,m_nExDataSLen,serial);

        m_pVHFLayer->DataLayerSendMemoryGather((char*)m_pExDataSend,m_nExDataSLen);
    }

    return t;
}




void  CE_VHFNodeManage::ReSetListCountNum()
{
//    EnterCriticalSection(&m_hDlgCritical);

//// 	LARGE_INTEGER litmp1;
//// 	QueryPerformanceFrequency(&litmp1);
//// 	double frezValue = (double)litmp1.QuadPart;
////
//// 	QueryPerformanceCounter(&litmp1);
//// 	LONGLONG Qpart1 = litmp1.QuadPart;
//    LONGLONG count1 = GetTickCount();


//    POSITION pos = m_lVHFMsgList.GetHeadPosition();
//    POSITION prvpos;
//    while(pos)
//    {
//        prvpos = pos;
//        CSCRSC_ObjVHFMsg& msg = m_lVHFMsgList.GetNext(pos);
//        msg.nTimeCount = 0;
//    }

//// 	LARGE_INTEGER litmp2;
//// 	LONGLONG Qpart2;
//// 	QueryPerformanceCounter(&litmp2);
//// 	Qpart2 = litmp2.QuadPart;
//    LONGLONG count2 = GetTickCount();
//    TRACE(_T("ReSetListCountNum %d \r\n"),count2 - count1);
//    LeaveCriticalSection(&m_hDlgCritical);
}

bool CE_VHFNodeManage::DeleteACCtoRSCMessageData(const int nSendID, const int nSerialBegin, const int nSerialEnd)
{
    QMutexLocker locker(&m_listMutex);
    bool bFinder = false;

    QList<pVHFMsg>::iterator iter = m_lVHFMsgList.begin();
    while (iter != m_lVHFMsgList.end())
    {
        if (((*iter)->nSource == nSendID) && \
            ((*iter)->nSerial >= nSerialBegin) && \
            ((*iter)->nSerial <= nSerialEnd) )
        {
            bFinder = true;
            if ((*iter)->nSerial == nSerialEnd)
            {
                m_lVHFMsgList.erase(iter);
                break;
            }
            else
            {
                iter = m_lVHFMsgList.erase(iter);
            }
        }
        else
        {
            ++iter;
        }
    }
    return bFinder;
}

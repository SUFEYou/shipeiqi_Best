#include "CSC_01LayerHead.h"
#include "common.h"
#include <VHFLayer/CE_VHFNodeManage.h>
#include <VHFLayer/CSC_01LayerClient.h>
#include <QTime>
#include <QDebug>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSC_01LayerHead::CSC_01LayerHead()
{
    m_pDataControl	= new char[512];// Control Message Save
    m_pDataCtlLen	= 0;			// Control Message length

    m_nDataMaxLen = PACK_LENGTHLIMIT;

    //////////////////////////////////////////////////////////////////////////
    // Chain Member is Change
    m_bChainChange	= false;		// Chain is or Not Change
    m_bMainChange	= false;
    m_bMainCode	= 0;

}

CSC_01LayerHead::~CSC_01LayerHead()
{
    if (m_pDataControl)
    {
        delete []m_pDataControl;
        m_pDataControl = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////
//	Interface of Data
//////////////////////////////////////////////////////////////////////////

// Input the Layer Information
void CSC_01LayerHead::UseDataInput(const char* pchar,const int nlength)
{
//	UseSaveReceiveBytes(pchar,nlength);
    int i= 0;
    for (i = 0; i<nlength; i++)
    {
        ActAnalyzeVHFLayer(*(pchar+i));
    }
}

//////////////////////////////////////////////////////////////////////////
//
// Analyze Data Layer Char
//
//////////////////////////////////////////////////////////////////////////
// Analyze the Sentence
void CSC_01LayerHead::ActAnalyzeVHFLayer(unsigned char cRec)
{
    switch(m_CFlag)
    {
    case 0:
        if(cRec == 0xAA)		// Get the Start Char
        {
            m_CFlag=1;
            m_CPos=1;
            m_CReadBuf[0] = 0xAA;
        }
        break;
    case 1:
        if (cRec == 0xAA)
        {
            m_CFlag = 1;
            m_CPos = 1;
            m_CReadBuf[0] = 0xAA;
        }
        else
        {
            m_CFlag = 2;
            m_CReadBuf[m_CPos++] = cRec;
        }
        break;
    case 2:
        m_CReadBuf[m_CPos++] = cRec;
        if (cRec == 0xAA)
        {
            // Judge the Data Length
            if (m_CPos > 10)
            {
                // Analyze the Receive Data
                DataLayerMessageAnalyze(m_CReadBuf,m_CPos);

                m_CFlag=1;
                m_CPos=1;
                m_CReadBuf[0] = 0xAA;

            }
            else
            {
                m_CFlag = 0;
                m_CPos  = 0;
            }

        }
        else
        {
            // Out the Length Limit
            if (m_CPos > 1000)
            {
                m_CFlag = 0;
                m_CPos  = 0;
            }
        }
        break;
    default:
        break;
    }
}

void CSC_01LayerHead::ActAnalyzeError()
{
    m_CFlag=0;
}

//////////////////////////////////////////////////////////////////////////
// Analyze & Pack the Sentence
//////////////////////////////////////////////////////////////////////////
// VHF Layer Control Sentence
// Analyze the Layer Sentence
bool CSC_01LayerHead::DataLayerMessageAnalyze(char* pchar,const int nlength)
{

    if (!DataLayerMessageParse(pchar,nlength,m_nRecvMsg))
    {
        return false;
    }

    LinkLayerChainDealWithRecvMsg(m_nRecvMsg.nSource);

    if (!m_bMonitorAll)
    {
        // It isn't Myself Information
        if (m_nRecvMsg.nReceive != m_nCodeMe &&
            m_nRecvMsg.nReceive != m_nChain.nChainId &&
            m_nRecvMsg.nReceive != BROADCAST_ID)
        {
            return false;
        }
    }

    if (m_nRecvMsg.nSource == m_nCodeMe)
    {
        return false;
    }

    // Judge the Information
    switch(m_nRecvMsg.pData[0])
    {
    case LAYMSG_CONTROL:
        {
            if (ActSenLAYMSG_CONTROLUnpack(m_nRecvMsg.pData,m_nRecvMsg.nDataLen))
            {
            // 上报当前链表状态
                CE_VHFNodeManage::getInstance()->RSCtoACCChainState();
                if (NULL != CE_VHFNodeManage::getInstance()->m_pLayerVHFClient)
                {
                    SetAvailable(false);
                    CE_VHFNodeManage::getInstance()->m_pLayerVHFClient->m_nChain	= m_nRecvChain;
                    CE_VHFNodeManage::getInstance()->m_pLayerVHFClient->m_nTNotInChainCt = 0;
                    CE_VHFNodeManage::getInstance()->m_pLayerVHFClient->SetAvailable(true);
                    CE_VHFNodeManage::getInstance()->m_pLayerVHFClient->m_nMeState = 0;

                    CE_VHFNodeManage::getInstance()->m_pLayerVHFClient->m_nChainIDGo = m_nRecvChain.nChainId;
                    CE_VHFNodeManage::getInstance()->m_pLayerVHFClient->m_bSendOK        = false;
                    CE_VHFNodeManage::getInstance()->m_pLayerVHFClient->LinkLayerCircleMomentToCircle();
                    CE_VHFNodeManage::getInstance()->m_pLayerVHFClient->m_nMeState = 0;
                }
                else
                {
                    m_nChain = m_nRecvChain;
                    m_nChainIDNow	= m_nChain.nChainId;
                    m_nSeatNow = m_nRecvMsg.nSource;
                    m_bSendOK  = false;

                    LinkLayerCircleMomentToCircle();
                    LinkLayerChainDealWithRecvMsg(m_nSeatNow);
                }

                if (m_bMainChange)
                {
                    if (m_bMainCode == m_nRecvMsg.nSource)
                    {

                        CE_VHFNodeManage::getInstance()->m_pLayerVHFClient->m_nChain	= m_nRecvChain;
                        CE_VHFNodeManage::getInstance()->m_pLayerVHFClient->SetAvailable(true);
                        SetAvailable(false);
                        return true;

                    }
                }

                qDebug() << QString("%1=>LAYMSG_CONTROL").arg(m_nRecvMsg.nSource);
                qDebug() << "Head LAYMSG_CONTROL";
            }
        }
        break;
    case LAYMSG_STATE:
        {
            qDebug() << "Head LAYMSG_STATE";
            if (ActSenLAYMSG_STATEUnpack(m_nRecvMsg.pData,m_nRecvMsg.nDataLen))
            {
                // Do with the Apply
                if (m_nRecvState == LAYAPP_ONLINE)
                {
                    LinkLayerChainDealWithStageApply(m_nRecvMsg.nSource,m_nRecvState);

                    // Send to main exchange Class that one Apply enjoin the chain
                    qDebug() << QString("%1=>LAYAPP_ONLINE").arg(m_nRecvMsg.nSource);
                    qDebug() << "Head LAYMSG_STATE  LAYAPP_ONLINE";

                }
                else if(m_nRecvState == LAYAPP_STILL)	// Still Online
                {
                    qDebug() << QString("%1=>LAYAPP_STILL").arg(m_nRecvMsg.nSource);
                    qDebug() << "Head LAYMSG_STATE  LAYAPP_STILL";
                }
                else if(m_nRecvState == LAYAPP_OFFLINE)	// offline
                {
                    LinkLayerChainDealWithStageApply(m_nRecvMsg.nSource,m_nRecvState);
                    qDebug() << QString("%1=>LAYAPP_OFFLINE").arg(m_nRecvMsg.nSource);
                    qDebug() << "Head LAYMSG_STATE  LAYAPP_OFFLINE";
                }
                else
                {
                    break;
                }
            }
        }
        break;
    case LAYMSG_MSGCAST:
        {
            if (ActSenLAYMSG_MSGCASTUnpack(m_nRecvMsg.pData,m_nRecvMsg.nDataLen))
            {
                // to Main Exchange Class do with the Data
                // 处理接收到的广播报文
                CE_VHFNodeManage::getInstance()->PackToSendRMTtoRSCMessageData(m_nRecvMsg.nSource,m_nRecvMsg.nReceive,
                                                                               m_pMsgRecvData,m_pMsgRecvLen,true);

                qDebug() << QString("%1=>LAYMSG_MSGCAST").arg(m_nRecvMsg.nSource);
            }
        }
        break;
    case LAYMSG_MSGONCE:
        {
            if (ActSenLAYMSG_MSGONCEUnpack(m_nRecvMsg.pData,m_nRecvMsg.nDataLen))
            {
                // Add the Recall
                pCSCObjRecall msg(new CSCObjRecall);
                msg->nSource = m_nRecvMsg.nSource;
                msg->nSerial = m_pMsgRecvSn;
                m_nListRecall.push_back(msg);

                qDebug() << QString("HEAD LAYMSG_MSGONCE msg.nSource %1 msg.nSerial %2").arg(msg->nSource).arg(msg->nSerial);

                // to Main Exchange Class do with the Data
                // 处理接收到的报文

                CE_VHFNodeManage::getInstance()->PackToSendRMTtoRSCMessageData(m_nRecvMsg.nSource,m_nRecvMsg.nReceive,
                                                                               m_pMsgRecvData,m_pMsgRecvLen,false);

                qDebug() << QString("%1=>LAYMSG_MSGONCE").arg(m_nRecvMsg.nSource);
            }
        }
        break;
    case LAYMSG_MSGCALL:
        {
            qDebug() << "ActSenLAYMSG_MSGCALLUnpack " << m_nRecvMsg.nSerial;
            if (ActSenLAYMSG_MSGCALLUnpack(m_nRecvMsg.pData,m_nRecvMsg.nDataLen))
            {
                for (int i = 0; i < m_nRecvCallList.length(); ++i)
                {
                    CE_VHFNodeManage::getInstance()->RMTtoRSCMessageSerial(m_nRecvCallList[i]->nSource,m_nRecvCallList[i]->nSerial);
                }

                CE_VHFNodeManage::getInstance()->ReSetListCountNum();

                qDebug() << QString("%1=>LAYMSG_MSGCALL").arg(m_nRecvMsg.nSource);
            }
        }
        break;
    default:
        break;
    }


    // Monitor State
    if (m_nMonCode == m_nRecvMsg.nSource)
    {
        m_nMonRecvTime = QTime::currentTime();
    }

    //////////////////////////////////////////////////////////////////////////
//    if (m_pMain)
//    {
//        ((CCE_VHFNodeDlg*)m_pMain)->VHFLayerReportControlMessage(m_nRecvMsg.pData[0],strDesc);
//    }

    return true;

}

//////////////////////////////////////////////////////////////////////////
// Moment Control Operate
// Timer Control Interface
void CSC_01LayerHead::XTimeMaintenanceCircle()
{
    LinkLayerMainCircle();
}

//////////////////////////////////////////////////////////////////////////
//
// Link Layer Control
//
//////////////////////////////////////////////////////////////////////////
// Link Layer Circle
// Main Control
void CSC_01LayerHead::LinkLayerMainCircle()
{
    switch(m_nMoment)
    {
    case MOMENT_BEGIN:
        {
            //////////////////////////////////////////////////////////////////////////
            // Head Pack the Chain, Send Out Self Data, Enter Circle Moment
        }
        break;
    case MOMENT_CIRCLE:
        {
            //////////////////////////////////////////////////////////////////////////
            // Wait for the time to Send Out Data
            m_nTOutCount++;
            qDebug() << "Head MOMENT_CIRCLE";
            if (m_nTOutCount >= m_nChain.nLimitOut*m_nTimeFactor)
            {
                qDebug() << "Head MOMENT_CIRCLE  m_nTOutCount >= m_nChain.nLimitOut*m_nTimeFactor";
                LinkLayerChainDealWithOuttime(m_nSeatNow);
            }
        }
        break;
    case MOMENT_APPLY:
        {
            //////////////////////////////////////////////////////////////////////////
            // Send What
            qDebug() << "Head MOMENT_APPLY";
            m_nTOutCount++;
            if (m_bSendApplyCan)
            {
                if (m_nTOutCount >= m_nTApply)
                {
                    // Send the Apply Information
                    LinkLayerComSendApplyData();
                    qDebug() << "Head MOMENT_APPLY LinkLayerComSendApplyData";
                }
            }

            if (m_nTOutCount >= m_nChain.nLimitApply*m_nTimeFactor)
            {
                LinkLayerCircleMomentToDrift();
                qDebug() << "Head MOMENT_APPLY LinkLayerCircleMomentToDrift";
            }

            // Chain is Changed
            if (m_bChainChange)
            {
                m_nChain = m_nChainTmp;
                m_bChainChange = false;
            }
        }
        break;
    case MOMENT_DRIFT:
        {
            qDebug() << "Head MOMENT_DRIFT";
            m_nTOutCount++;
            if (m_nTOutCount >= m_nChain.nLimitDrift*m_nTimeFactor)
            {
                m_bChainCircleFlag = false;
                m_bSendOK = false;
                LinkLayerCircleMomentToBegin();

                qDebug() << "Head MOMENT_DRIFT LinkLayerCircleMomentToBegin";
            }

        }
        break;
    default:
        break;
    }
}

// Send Message
void CSC_01LayerHead::LinkLayerComSendMemoryData()
{
    if (m_bSendOK)
    {
        return;
    }


    // Add the Data into Send Memory
    memset(m_pDSendData,0,PACK_LENGTHLIMIT);
    m_pDSendLen	 = 0;


    // Add the Recall Information
    if (ActSenLAYMSG_MSGCALLPack(0))
    {
        CSCRSC_ObjVHFMsg msgrecall;
        msgrecall.bEncrypt = false;
        msgrecall.nDataLen = m_pExDataLen;
        memcpy(msgrecall.pData,m_pExData,m_pExDataLen);
        msgrecall.nReceive = BROADCAST_ID;
        msgrecall.nVersion = m_nCodeVer;
        msgrecall.nSource  = m_nCodeMe;
        DataLayerMessageEncode(msgrecall,m_pExData,m_pExDataLen);

        DataLayerSendMemoryGather(m_pExData,m_pExDataLen);
    }

    //////////////////////////////////////////////////////////////////////////
    // Control Message
    // Create Control Message and Send
    memset(m_pDataControl,0,512);
    m_pDataCtlLen = 0;

    QList<pCSCObjStage>::iterator iter = m_nChain.nListMember.begin();
    QList<pCSCObjStage>::iterator deletePos;
    while (iter != m_nChain.nListMember.end())
    {
        deletePos = iter;
        pCSCObjStage stg = *iter;
        ++iter;
        if (LAYSTA_OFFLINE == stg->state)
        {
            if (deletePos != m_nChain.nListMember.end())
            {
                m_nChain.nListMember.erase(deletePos);
                if(m_nChain.nMemNum > 0)
                {
                    --m_nChain.nMemNum;
                }
            }
        }
    }

    ActSenLAYMSG_CONTROLPack(m_nChain);
    CSCRSC_ObjVHFMsg msg;
    msg.bEncrypt = false;
    msg.nDataLen = m_pExDataLen;
    memcpy(msg.pData,m_pExData,m_pExDataLen);
    msg.nReceive = BROADCAST_ID;
    msg.nVersion = m_nCodeVer;
    msg.nSource  = m_nCodeMe;
    DataLayerMessageEncode(msg,m_pDataControl,m_pDataCtlLen);

    // 上报当前链表状态

    CE_VHFNodeManage::getInstance()->RSCtoACCChainState();
    qDebug() << QString("%1<=LAYAPP_CONTROL").arg(m_nCodeMe);

    //////////////////////////////////////////////////////////////////////////
    // Send the Must to been Send's Information in the Send List
    CE_VHFNodeManage::getInstance()->VHFLayerSendDataFromListWait(m_nDataMaxLen-m_pDataCtlLen);

    DataLayerSendMemoryGather(m_pDataControl,m_pDataCtlLen);
    // Send Data to the Layer
    // Message
    m_bSendOK = ComSendOutData(m_pDSendData,m_pDSendLen);

    qDebug() << "HEAD ComSendOutData SendLen " << m_pDSendLen;

}


void CSC_01LayerHead::LinkLayerComSendApplyData()
{
    // Send the Apply Moment Data

    if (m_bSendApplyCan)
    {
        ComSendOutData(m_pDApplyData,m_pDApplyLen);
        m_bSendApplyCan	= false;
    }

}

// Layer Into Begin Moment
void CSC_01LayerHead::LinkLayerCircleMomentToBegin()
{
    m_nMoment	= MOMENT_BEGIN;
    m_nTOutCount = 0;
    LinkLayerComSendMemoryData();
    LinkLayerComStepTouch(m_nCodeMe);
    LinkLayerCircleMomentToCircle();


}

// Layer Into Circle Moment
void CSC_01LayerHead::LinkLayerCircleMomentToCircle()
{
    m_nMoment	= MOMENT_CIRCLE;
    m_bChainCircleFlag		= true;		// Is or Out the Circle
    m_nTOutCount	= 0;

    m_nChainIDNow	= m_nChain.nChainId;

}

// Layer Into Apply Moment
void CSC_01LayerHead::LinkLayerCircleMomentToApply()
{
    m_nMoment	= MOMENT_APPLY;
    m_nTOutCount	= 0;
}

// Layer Into Drift Moment
void CSC_01LayerHead::LinkLayerCircleMomentToDrift()
{
    m_nMoment	= MOMENT_DRIFT;
    m_nTOutCount	= 0;
}

// Do with the Stage Apply
void CSC_01LayerHead::LinkLayerChainDealWithStageApply(int& nid,int& napply)
{
    if (nid <= 0)
    {
        return;
    }

    for (int i = 0; i < m_nChain.nListMember.length(); ++i)
    {
        pCSCObjStage obj = m_nChain.nListMember[i];
        if (obj->id == nid)
        {
            if (napply == LAYAPP_ONLINE)
            {
                obj->state = LAYSTA_ONLINE;
            }
            else if (napply == LAYAPP_OFFLINE)
            {
                obj->state = LAYSTA_OFFLINE;
            }
            else if (napply == LAYAPP_ONCE)
            {
                obj->state = LAYSTA_ONLINE;
            }
            else
            {
                obj->state = LAYSTA_ONLINE;
            }

            return;
        }
    }

    pCSCObjStage newobj(new CSCObjStage);
    newobj->id = nid;
    newobj->degree = 2;
    newobj->errcount = 0;
    newobj->signal = LAYSTA_SUCCESS;
    if (napply == LAYAPP_ONLINE)
    {
        newobj->state = LAYSTA_ONLINE;
    }
    else if (napply == LAYAPP_OFFLINE)
    {
        newobj->state = LAYSTA_OFFLINE;
    }
    else if (napply == LAYAPP_ONCE)
    {
        newobj->state = LAYSTA_ONLINE;
    }

    m_nChain.nListMember.push_back(newobj);

}

// Do With the Out Time
void CSC_01LayerHead::LinkLayerChainDealWithOuttime(int& nid)
{
    if (m_nMoment == MOMENT_CIRCLE)
    {
        m_nTOutCount = 0;

        LinkLayerChainSetStageOuttime(nid);

        LinkLayerComStepTouch(nid);


        if (m_nSeatNow == 0)
        {
            LinkLayerCircleMomentToApply();
        }
    }
}

// Do With Receive the Stage's Message
void CSC_01LayerHead::LinkLayerChainDealWithRecvMsg(int& nid)
{
    // UpdateData Stage's State
    LinkLayerChainSetStageRecvOk(nid);

    if (m_nMoment == MOMENT_CIRCLE)
    {
        if (m_nSeatPrv == nid)
        {
            return;
        }
        else
        {
            // Find Next One
            if (LinkLayerComStepTouch(nid))
            {
                m_nTOutCount = 0;
            }
        }

        // the Last One is Received
        if (m_nSeatNow == 0)
        {
            LinkLayerCircleMomentToApply();
        }
    }
    else
    {
        if (m_nSeatPrv == nid)
        {
            return;
        }
        else
        {
            m_nSeatPrv = nid;
        }
    }

}

//////////////////////////////////////////////////////////////////////////
// Update the List State
//////////////////////////////////////////////////////////////////////////
// Set the State
void CSC_01LayerHead::LinkLayerChainSetStageRecvOk(int& nid)
{
    for (int i = 0; i < m_nChain.nListMember.length(); ++i)
    {
        pCSCObjStage obj = m_nChain.nListMember[i];
        if (obj->id == nid)
        {
            obj->signal = LAYSTA_SUCCESS;
            obj->errcount = 0;
            return;
        }
    }
}

// Set the Stage Out Time
void CSC_01LayerHead::LinkLayerChainSetStageOuttime(int& nid)
{
    qDebug() << "Head  LinkLayerChainSetStageOuttime";
    for (int i = 0; i < m_nChain.nListMember.length(); ++i)
    {
        pCSCObjStage obj = m_nChain.nListMember[i];
        if (obj->id == nid)
        {
            obj->signal = LAYSTA_LOST;
            obj->errcount++;
            if (obj->errcount > 3)
            {
                obj->state = LAYSTA_OFFLINE;
            }
            return;
        }
    }
}

// Layer Control
void CSC_01LayerHead::LinkLayerChainMainChange(int nmain)
{
    m_bMainChange	= true;
    m_bMainCode		= nmain;
}

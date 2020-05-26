#include "RadioLinkMaster.h"
#include "RadioLinkManage.h"
#include "RadioLinkClient.h"
#include <QDebug>

RadioLinkMaster::RadioLinkMaster()
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

RadioLinkMaster::~RadioLinkMaster()
{
    if (m_pDataControl)
    {
        delete []m_pDataControl;
        m_pDataControl = NULL;
    }
}

void RadioLinkMaster::recvDataAnalyze(ObjMsg &recvMsg)
{
    LinkLayerChainDealWithRecvMsg(recvMsg.nSource);

    if (!m_bMonitorAll)
    {
        // It isn't Myself Information
        if (recvMsg.nReceive != m_nCodeMe &&
            recvMsg.nReceive != m_nChain.nChainId &&
            recvMsg.nReceive != BROADCAST_ID)
        {
            return ;
        }
    }

    if (recvMsg.nSource == m_nCodeMe)
    {
        return ;
    }

    // Judge the Information
    switch(recvMsg.pData[0])
    {
    case LAYMSG_CONTROL:
        {
            if (ActSenLAYMSG_CONTROLUnpack(recvMsg.pData,recvMsg.nDataLen))
            {
            // 上报当前链表状态
                RadioLinkManage::getInstance()->RSCtoACCChainState();
                RadioLinkClient* tmpClient = RadioLinkManage::getInstance()->getRadioLinkClient();
                if (NULL != tmpClient)
                {
                    SetAvailable(false);
                    tmpClient->setModelChain(m_nRecvChain);
                    tmpClient->setNotInChainCt(0);
                    tmpClient->SetAvailable(true);
                    tmpClient->setMeState(0);

                    tmpClient->setChainIDGo(0);
                    tmpClient->setSendOK(false);
                    tmpClient->LinkLayerCircleMomentToCircle();
                    tmpClient->setMeState(0);
                }
                else
                {
                    m_nChain = m_nRecvChain;
                    m_nChainIDNow	= m_nChain.nChainId;
                    m_nSeatNow = recvMsg.nSource;
                    m_bSendOK  = false;

                    LinkLayerCircleMomentToCircle();
                    LinkLayerChainDealWithRecvMsg(m_nSeatNow);
                }

                if (m_bMainChange)
                {
                    if (m_bMainCode == recvMsg.nSource)
                    {

                        tmpClient->setModelChain(m_nRecvChain);
                        tmpClient->SetAvailable(true);
                        SetAvailable(false);
                        return ;

                    }
                }

                qDebug() << QString("%1=>LAYMSG_CONTROL").arg(recvMsg.nSource);
                qDebug() << "Head LAYMSG_CONTROL";
            }
        }
        break;
    case LAYMSG_STATE:
        {
            qDebug() << "Head LAYMSG_STATE";
            if (ActSenLAYMSG_STATEUnpack(recvMsg.pData,recvMsg.nDataLen))
            {
                // Do with the Apply
                if (m_nRecvState == LAYAPP_ONLINE)
                {
                    LinkLayerChainDealWithStageApply(recvMsg.nSource,m_nRecvState);

                    // Send to main exchange Class that one Apply enjoin the chain
                    qDebug() << QString("%1=>LAYAPP_ONLINE").arg(recvMsg.nSource);
                    qDebug() << "Head LAYMSG_STATE  LAYAPP_ONLINE";

                }
                else if(m_nRecvState == LAYAPP_STILL)	// Still Online
                {
                    qDebug() << QString("%1=>LAYAPP_STILL").arg(recvMsg.nSource);
                    qDebug() << "Head LAYMSG_STATE  LAYAPP_STILL";
                }
                else if(m_nRecvState == LAYAPP_OFFLINE)	// offline
                {
                    LinkLayerChainDealWithStageApply(recvMsg.nSource,m_nRecvState);
                    qDebug() << QString("%1=>LAYAPP_OFFLINE").arg(recvMsg.nSource);
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
            if (ActSenLAYMSG_MSGCASTUnpack(recvMsg.pData,recvMsg.nDataLen))
            {
                // to Main Exchange Class do with the Data
                // 处理接收到的广播报文
                RadioLinkManage::getInstance()->PackToSendRMTtoRSCMessageData(recvMsg.nSource,recvMsg.nReceive,
                                                                               m_pMsgRecvData,m_pMsgRecvLen,true);

                qDebug() << QString("%1=>LAYMSG_MSGCAST").arg(recvMsg.nSource);
            }
        }
        break;
    case LAYMSG_MSGONCE:
        {
            if (ActSenLAYMSG_MSGONCEUnpack(recvMsg.pData,recvMsg.nDataLen))
            {
                // Add the Recall
                pObjRecall msg(new ObjRecall);
                msg->nSource = recvMsg.nSource;
                msg->nSerial = m_pMsgRecvSn;
                m_nListRecall.push_back(msg);

                qDebug() << QString("HEAD LAYMSG_MSGONCE msg.nSource %1 msg.nSerial %2").arg(msg->nSource).arg(msg->nSerial) << " nVersion " << recvMsg.nVersion;

                // to Main Exchange Class do with the Data
                // 处理接收到的报文

                RadioLinkManage::getInstance()->PackToSendRMTtoRSCMessageData(recvMsg.nSource,recvMsg.nReceive,
                                                                               m_pMsgRecvData,m_pMsgRecvLen,false);

                qDebug() << QString("%1=>LAYMSG_MSGONCE").arg(recvMsg.nSource);
            }
        }
        break;
    case LAYMSG_MSGCALL:
        {
            qDebug() << "ActSenLAYMSG_MSGCALLUnpack " << recvMsg.nSerial;
            if (ActSenLAYMSG_MSGCALLUnpack(recvMsg.pData,recvMsg.nDataLen))
            {
                for (int i = 0; i < m_nRecvCallList.length(); ++i)
                {
                    RadioLinkManage::getInstance()->RMTtoRSCMessageSerial(m_nRecvCallList[i]->nSource,m_nRecvCallList[i]->nSerial);
                }

                RadioLinkManage::getInstance()->ReSetListCountNum();

                qDebug() << QString("%1=>LAYMSG_MSGCALL").arg(recvMsg.nSource);
            }
        }
        break;
    default:
        break;
    }


    // Monitor State
    if (m_nMonCode == recvMsg.nSource)
    {
        m_nMonRecvTime = QTime::currentTime();
    }
}


// Layer Into Begin Moment
void RadioLinkMaster::LinkLayerCircleMomentToBegin()
{
    m_nMoment	= MOMENT_BEGIN;
    m_nTOutCount = 0;
    LinkLayerComSendMemoryData();
    LinkLayerComStepTouch(m_nCodeMe);
    LinkLayerCircleMomentToCircle();
}

// Layer Into Circle Moment
void RadioLinkMaster::LinkLayerCircleMomentToCircle()
{
    m_nMoment	= MOMENT_CIRCLE;
    m_bChainCircleFlag		= true;		// Is or Out the Circle
    m_nTOutCount	= 0;
    m_nChainIDNow	= m_nChain.nChainId;
}

// Layer Into Apply Moment
void RadioLinkMaster::LinkLayerCircleMomentToApply()
{
    m_nMoment	= MOMENT_APPLY;
    m_nTOutCount	= 0;
}

// Layer Into Drift Moment
void RadioLinkMaster::LinkLayerCircleMomentToDrift()
{
    m_nMoment	= MOMENT_DRIFT;
    m_nTOutCount	= 0;
}

void RadioLinkMaster::LinkLayerMainCircle()
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
            //qDebug() << "Head MOMENT_CIRCLE";
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
            //qDebug() << "Head MOMENT_APPLY";
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
            //qDebug() << "Head MOMENT_DRIFT";
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
void RadioLinkMaster::LinkLayerComSendMemoryData()
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
        ObjMsg msgrecall;
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

    QList<pObjStage>::iterator iter = m_nChain.nListMember.begin();

    while(iter != m_nChain.nListMember.end())
    {
        if (LAYSTA_OFFLINE == (*iter)->state)
        {
            iter = m_nChain.nListMember.erase(iter);
            if(m_nChain.nMemNum > 0)
            {
                --m_nChain.nMemNum;
            }
        }
        else
        {
            ++iter;
        }
    }

    ActSenLAYMSG_CONTROLPack(m_nChain);
    ObjMsg msg;
    msg.bEncrypt = false;
    msg.nDataLen = m_pExDataLen;
    memcpy(msg.pData,m_pExData,m_pExDataLen);
    msg.nReceive = BROADCAST_ID;
    msg.nVersion = m_nCodeVer;
    msg.nSource  = m_nCodeMe;
    DataLayerMessageEncode(msg,m_pDataControl,m_pDataCtlLen);

    // 上报当前链表状态

    RadioLinkManage::getInstance()->RSCtoACCChainState();
    qDebug() << QString("%1<=LAYAPP_CONTROL").arg(m_nCodeMe);

    //////////////////////////////////////////////////////////////////////////
    // Send the Must to been Send's Information in the Send List
    RadioLinkManage::getInstance()->sendDataFromListWait(m_nDataMaxLen-m_pDataCtlLen);

    DataLayerSendMemoryGather(m_pDataControl,m_pDataCtlLen);
    // Send Data to the Layer
    // Message
    m_bSendOK = ComSendOutData(m_pDSendData,m_pDSendLen);

    qDebug() << "HEAD ComSendOutData SendLen " << m_pDSendLen;

}


void RadioLinkMaster::LinkLayerComSendApplyData()
{
    // Send the Apply Moment Data

    if (m_bSendApplyCan)
    {
        ComSendOutData(m_pDApplyData,m_pDApplyLen);
        m_bSendApplyCan	= false;
    }

}

// Do with the Stage Apply
void RadioLinkMaster::LinkLayerChainDealWithStageApply(int& nid,int& napply)
{
    if (nid <= 0)
    {
        return;
    }

    for (int i = 0; i < m_nChain.nListMember.length(); ++i)
    {
        pObjStage obj = m_nChain.nListMember[i];
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

    pObjStage newobj(new ObjStage);
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
void RadioLinkMaster::LinkLayerChainDealWithOuttime(int& nid)
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
void RadioLinkMaster::LinkLayerChainDealWithRecvMsg(int& nid)
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
void RadioLinkMaster::LinkLayerChainSetStageRecvOk(int& nid)
{
    for (int i = 0; i < m_nChain.nListMember.length(); ++i)
    {
        pObjStage obj = m_nChain.nListMember[i];
        if (obj->id == nid)
        {
            obj->signal = LAYSTA_SUCCESS;
            obj->errcount = 0;
            return;
        }
    }
}

// Set the Stage Out Time
void RadioLinkMaster::LinkLayerChainSetStageOuttime(int& nid)
{
    qDebug() << "Head  LinkLayerChainSetStageOuttime";
    for (int i = 0; i < m_nChain.nListMember.length(); ++i)
    {
        pObjStage obj = m_nChain.nListMember[i];
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
void RadioLinkMaster::LinkLayerChainMainChange(int nmain)
{
    m_bMainChange	= true;
    m_bMainCode		= nmain;
}

void RadioLinkMaster::timerProcess()
{
    LinkLayerMainCircle();
}

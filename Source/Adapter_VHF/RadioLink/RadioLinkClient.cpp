#include "RadioLinkClient.h"
#include "RadioLinkManage.h"
#include "config/ConfigLoader.h"
#include <QDebug>
#include "log/log4z.h"

using namespace zsummer::log4z;

RadioLinkClient::RadioLinkClient()
{
    m_nDataMaxLen = PACK_LENGTHLIMIT;
    m_bMsgStill	= false;		// Message Still
    m_nTNotInChainCt	= 0;	// 不在链表中的时间汇总
    m_nTNotInChainCtLmt	= 300;	// 不在链接中的时间界限
    m_IsInChainWhereNum = 0;
}

RadioLinkClient::~RadioLinkClient()
{

}

void RadioLinkClient::recvDataAnalyze(ObjMsg &recvMsg)
{
    if (recvMsg.nSource == m_nCodeMe)
    {
        return;
    }
    // Do with the Receive Stage
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
    //过滤接收信息不为本艇或广播直接舍弃
    if (recvMsg.nReceive != m_nCodeMe && recvMsg.nReceive != BROADCAST_ID)
        return;

    QString strDesc;
    // Judge the Information
    switch(recvMsg.pData[0])
    {
    case LAYMSG_CONTROL:
        {
            LOGD("In RadioLinkClient::recvDataAnalyze, Recv CONTROL");
            if (ActSenLAYMSG_CONTROLUnpack(recvMsg.pData,recvMsg.nDataLen))
            {
                // Judge is in the Chain & Can Send
                bool bIn = m_nRecvChain.IsInList(m_nCodeMe,m_nMeState);
                if ( bIn == true || m_nRecvChain.nChainId == m_nChainIDGo)
                {
                    m_nChain = m_nRecvChain;
                    m_nChainIDNow	= recvMsg.nSource;
                    m_nSeatNow = recvMsg.nSource;
                    m_bSendOK = false;

                    RecordInChainPosition(m_nChain,m_nCodeMe);
                    LinkLayerCircleMomentToCircle();
                    LinkLayerChainDealWithRecvMsg(m_nSeatNow);

                    // Send Apply
                    if (m_nMeState != LAYSTA_ONLINE)
                    {
                        DataLayerMessageStateApply(LAYAPP_ONLINE,QString::fromUtf8("入链！"));
                        m_bSendApplyCan	= true;
                    }

                    // 上报当前链表状态
                    RadioLinkManage::getInstance()->RSCtoACCChainState();

                }
                else // if (m_nChainIDGo == 0 && m_nChainIDNow == 0)
                {
                    m_nChainIDGo = m_nRecvChain.nChainId;
                    m_nTNotInChainCt = 0;
                    LinkLayerCircleMomentToCircle();
                }
            }
        }
        break;
    case LAYMSG_STATE:
        {
            LOGD("In RadioLinkClient::recvDataAnalyze, Recv STATE");
            if(m_nRecvState == LAYAPP_STILL)	// Still Online
            {
            }

            strDesc = QString("%1=>LAYMSG_STATE").arg(recvMsg.nSource);
        }
        break;
    case LAYMSG_MSGCAST:
        {
            LOGD("In RadioLinkClient::recvDataAnalyze, Recv CAST");
            if (ActSenLAYMSG_MSGCASTUnpack(recvMsg.pData,recvMsg.nDataLen))
            {
                // to Main Exchange Class do with the Data
                // 处理接收到的广播报文
                RadioLinkManage::getInstance()->PackToSendRMTtoRSCMessageData(recvMsg.nSource,recvMsg.nReceive, \
                                                                               m_pMsgRecvData,m_pMsgRecvLen,true);

                strDesc = QString("%1=>LAYMSG_MSGCAST").arg(recvMsg.nSource);
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
                LOGD(QString("In RadioLinkClient::recvDataAnalyze, Recv ONCE, Source %1, Serial %2").arg(msg->nSource).arg(msg->nSerial).toStdString().c_str());
                // to Main Exchange Class do with the Data
                // 处理接收到的报文
                RadioLinkManage::getInstance()->PackToSendRMTtoRSCMessageData(recvMsg.nSource,recvMsg.nReceive,\
                                                                               m_pMsgRecvData,m_pMsgRecvLen,false);

                strDesc = QString("%1=>LAYMSG_MSGONCE").arg(recvMsg.nSource);

            }
        }
        break;
    case LAYMSG_MSGCALL:
        {
            LOGD("In RadioLinkClient::recvDataAnalyze, Recv CALL");
            if (ActSenLAYMSG_MSGCALLUnpack(recvMsg.pData,recvMsg.nDataLen))
            {
                // to Main Exchange Class do with the data

                for (int i = 0; i < m_nRecvCallList.length(); ++i)
                {
                    RadioLinkManage::getInstance()->RMTtoRSCMessageSerial(m_nRecvCallList[i]->nSource,m_nRecvCallList[i]->nSerial);
                }

                RadioLinkManage::getInstance()->ReSetListCountNum();
                strDesc = QString("%1=>LAYMSG_MSGCALL").arg(recvMsg.nSource);
            }
        }
        break;
    default:
        break;
    }

    //////////////////////////////////////////////////////////////////////////
    // Monitor State
    if (m_nMonCode == recvMsg.nSource)
    {
        m_nMonRecvTime = QTime::currentTime();
    }
}

// Send Message
void RadioLinkClient::LinkLayerComSendMemoryData()
{
    if (m_bSendOK)
    {
        LOGD("return LinkLayerComSendMemoryData  m_bSendOK");
        return;
    }

    ///////////////////////////////////////////////////////////////////
    // Add Send Message Data
    memset(m_pDSendData,0,PACK_LENGTHLIMIT);
    m_pDSendLen	 = 0;

    int nRecall = RadioLinkManage::getInstance()->sendDataFromListWait(m_nDataMaxLen);
    if (nRecall == 1)
    {
        LOGD("Client nRecall");
    }

    // Still Online
    DataLayerMessageStateApply(LAYAPP_STILL,QString(""));	// use the space
    DataLayerSendMemoryGather(m_pDApplyData,m_pDApplyLen);

    qDebug() << QString("%1<=LAYAPP_STILL").arg(m_nCodeMe);

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

        qDebug() << QString("%1<=MSG_RECALL").arg(m_nCodeMe);

    }

    // Send Data to the Layer
    m_bSendOK = ComSendOutData(m_pDSendData,m_pDSendLen);
    LOGD(QString("In RadioLinkClient::LinkLayerComSendMemoryData, Send Len: %1").arg(m_pDSendLen).toStdString().c_str());
}

void RadioLinkClient::LinkLayerComSendApplyData()
{
    // Send the Apply Moment Data
    LOGD("Client  LinkLayerComSendApplyData");
    if (m_bSendApplyCan)
    {
        ComSendOutData(m_pDApplyData,m_pDApplyLen);
        m_bSendApplyCan = false;
    }
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
// Layer Into Begin Moment
void RadioLinkClient::LinkLayerCircleMomentToBegin()
{
    m_nMoment	= MOMENT_BEGIN;
    m_nTOutCount = 0;

    LinkLayerCircleMomentToCircle();
}

// Layer Into Circle Moment
void RadioLinkClient::LinkLayerCircleMomentToCircle()
{
    m_nMoment	= MOMENT_CIRCLE;
    m_bChainCircleFlag	= true;
    m_nTOutCount	= 0;
}

// Layer Into Apply Moment
void RadioLinkClient::LinkLayerCircleMomentToApply()
{
    m_nMoment	= MOMENT_APPLY;
    m_nTOutCount	= 0;
}

// Layer Into Drift Moment
void RadioLinkClient::LinkLayerCircleMomentToDrift()
{
    m_nMoment	= MOMENT_DRIFT;
    m_nTOutCount	= 0;
}

void RadioLinkClient::LinkLayerMainCircle()
{
    switch(m_nMoment)
    {
    case MOMENT_BEGIN:
        {
            ///////////////////////////////////////////////////////
            // Head Pack the Chain, Send Out Self Data, Enter Circle Moment
        }
        break;
    case MOMENT_CIRCLE:
        {
            //////////////////////////////////////////////////////
            // Wait for the time to Send Out Data
            m_nTOutCount++;

            if (m_bTCircleSend)
            {
                m_nTCircleDriftCount++;
                if (m_nTCircleDriftCount >= m_nTCircleDrift)
                {
                    ///////////////////////////////////////////////////////
                    // Send the Message
                    LOGD(QString("Client MOMENT_CIRCLE id %1").arg( m_nCodeMe).toStdString().c_str());
                    LinkLayerComSendMemoryData();
                    m_nTOutCount = 0;

                    // 当此CLIENT为最后一个CLient的时候发送后直接进入申请阶段与头一直
                    if (LinkLayerComStepTouch(m_nCodeMe))
                    {
                        if (m_nSeatNow == 0)
                        {
                            LinkLayerCircleMomentToApply();
                        }
                    }
                    m_bTCircleSend = false;
                }
            }

            if (m_nTOutCount >= m_nChain.nLimitOut*m_nTimeFactor)
            {
                LinkLayerChainDealWithOuttime(m_nSeatNow);
            }

        }
        break;
    case MOMENT_APPLY:
        {
            ///////////////////////////////////////////////////////
            // Send What
            m_nTOutCount++;
            if (m_bSendApplyCan)
            {
                if (m_nTOutCount >= m_nTApply)
                {
                    // Send the Apply Information
                    LinkLayerComSendApplyData();
                }
            }

            if (m_nTOutCount >= m_nChain.nLimitApply*m_nTimeFactor)
            {
                LinkLayerCircleMomentToDrift();
            }
        }
        break;
    case MOMENT_DRIFT:
        {
            // Judge the Client to Head Change Operate
            // Drift Time Out Count
            m_nTOutCount++;
            if (m_nTOutCount >= m_nChain.nLimitDrift*m_nTimeFactor)
            {
                m_bChainCircleFlag = false;
                m_nTNotInChainCt = -m_IsInChainWhereNum * 50;
                LOGD(QString("Client m_nTNotInChainCt %1  MOMENT_DRIFT m_nTOutCount >= m_nChain.nLimitDrift*m_nTimeFactor").arg(m_nTNotInChainCt).toStdString().c_str());
                m_nChain.Clear();		// Clear Chain
            }
        }
        break;
    default:
        break;
    }
}

// Do With the Out Time
void RadioLinkClient::LinkLayerChainDealWithOuttime(int& nid)
{
    if (m_nMoment == MOMENT_CIRCLE)
    {
        m_nTOutCount = 0;
        LinkLayerComStepTouch(nid);
    }

    // Judge Send Message State
    if (m_nSeatNow == m_nCodeMe)
    {
        LinkLayerComSendMemoryData();
        m_nTOutCount = 0;
        LinkLayerComStepTouch(m_nCodeMe);
    }
    else if (m_nSeatNow == 0)
    {
        LinkLayerCircleMomentToApply();
    }

}

// Do With Receive the Stage's Message
void RadioLinkClient::LinkLayerChainDealWithRecvMsg(int& nid)
{
    if (m_nMoment == MOMENT_CIRCLE)
    {
        if (m_nSeatPrv == nid)
        {
            return;
        }
        else
        {
            if (LinkLayerComStepTouch(nid))
            {
                m_nTOutCount = 0;
            }

            // Judge Send Message State
            if (m_nSeatNow == m_nCodeMe)
            {
                m_nTCircleDriftCount = 0;
                m_bTCircleSend = true;
                qDebug() << "m_bTCircleSend";
            }
            else if (m_nSeatNow == 0)
            {
                LinkLayerCircleMomentToApply();
            }

        }

        // the Last One is Received
        if (m_nSeatNow == 0)
        {
            LinkLayerCircleMomentToApply();
        }
        return;
    }
}

void RadioLinkClient::RecordInChainPosition(ModelChain& nChain,int nId)
{
    for (int i = 0; i < nChain.nListMember.length(); ++i)
    {
        pObjStage obj = nChain.nListMember[i];
        if (obj->id == nId)
        {
            m_IsInChainWhereNum = i;
            return;
        }
    }

    m_IsInChainWhereNum  = 0;
    return;
}

void RadioLinkClient::timerProcess()
{
    if (m_bChainCircleFlag)
    {
        m_nTNotInChainCt = 0;
        LinkLayerMainCircle();

    }
    else
    {
        // Not int the Chain
        m_nTNotInChainCt++;
        //A01不主动切换主台，只在A03实现超时切换主台
        if ((m_nTNotInChainCt >= m_nTNotInChainCtLmt) && (ConfigLoader::getInstance()->getSysType() == 0x0A03))
        {
            RadioLinkManage::getInstance()->changeClientToMaster();
        }
    }
}

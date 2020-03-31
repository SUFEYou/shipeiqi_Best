#include "CSC_01LayerClient.h"
#include "common.h"
#include "VHFLayer/CE_VHFNodeManage.h"
#include <QTime>
#include <QDebug>

CSC_01LayerClient::CSC_01LayerClient()
{
    m_nDataMaxLen = PACK_LENGTHLIMIT;
    m_bMsgStill	= false;		// Message Still
    m_nTNotInChainCt	= 0;	// 不在链表中的时间汇总
    m_nTNotInChainCtLmt	= 300;	// 不在链接中的时间界限
    m_IsInChainWhereNum = 0;
}

CSC_01LayerClient::~CSC_01LayerClient()
{

}

//////////////////////////////////////////////////////////////////////////
//	Interface of Data
//////////////////////////////////////////////////////////////////////////

// Input the Layer Information
void CSC_01LayerClient::UseDataInput(const char* pchar,const int nlength)
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
void CSC_01LayerClient::ActAnalyzeVHFLayer(unsigned char cRec)
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

void CSC_01LayerClient::ActAnalyzeError()
{
    m_CFlag=0;
}

//////////////////////////////////////////////////////////////////////////
// Analyze & Pack the Sentence
//////////////////////////////////////////////////////////////////////////
// VHF Layer Control Sentence
// Analyze the Layer Sentence
bool CSC_01LayerClient::DataLayerMessageAnalyze(char* pchar,const int nlength)
{

    if (!DataLayerMessageParse(pchar,nlength,m_nRecvMsg))
    {
        return false;
    }

    if (m_nRecvMsg.nSource == m_nCodeMe)
    {
        return false;
    }

    // Do with the Receive Stage
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

    QString strDesc;
    // Judge the Information
    switch(m_nRecvMsg.pData[0])
    {
    case LAYMSG_CONTROL:
        {
            //qDebug() << "Client  LAYMSG_CONTROL";
            if (ActSenLAYMSG_CONTROLUnpack(m_nRecvMsg.pData,m_nRecvMsg.nDataLen))
            {
                // Judge is in the Chain & Can Send
                bool bIn = m_nRecvChain.IsInList(m_nCodeMe,m_nMeState);
                if ( bIn == true || m_nRecvChain.nChainId == m_nChainIDGo)
                {
                    m_nChain = m_nRecvChain;
                    m_nChainIDNow	= m_nRecvMsg.nSource;
                    m_nSeatNow = m_nRecvMsg.nSource;
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
                    CE_VHFNodeManage::getInstance()->RSCtoACCChainState();

                }
                else // if (m_nChainIDGo == 0 && m_nChainIDNow == 0)
                {
                    m_nChainIDGo = m_nRecvChain.nChainId;
                    m_nTNotInChainCt = 0;
                    // Send Apply
//                    if (m_nMeState != LAYSTA_ONLINE)
//                    {
//                        qDebug() << "Client else DataLayerMessageStateApply";
//                        DataLayerMessageStateApply(LAYAPP_ONLINE,QString::fromUtf8("入链！"));
//                        m_bSendApplyCan	= true;
//                        m_bChainCircleFlag = true;
//                        LinkLayerCircleMomentToCircle();
//                    }
                    LinkLayerCircleMomentToCircle();
//                    if (m_nMeState != LAYSTA_ONLINE)
//                    {
//                        DataLayerMessageStateApply(LAYAPP_ONLINE,QString::fromUtf8("入链！"));
//                        m_bSendApplyCan	= true;
//                    }

                }
                //strDesc.Format(L"%d=>LAYMSG_CONTROL",m_nRecvMsg.nSource);
                strDesc = QString("%1=>LAYMSG_CONTROL").arg(m_nRecvMsg.nSource);
            }
        }
        break;
    case LAYMSG_STATE:
        {
            //qDebug() << "Client  LAYMSG_STATE";
            if(m_nRecvState == LAYAPP_STILL)	// Still Online
            {
            }

            //strDesc.Format(L"%d=>LAYMSG_STATE",m_nRecvMsg.nSource);
            strDesc = QString("%1=>LAYMSG_STATE").arg(m_nRecvMsg.nSource);
        }
        break;
    case LAYMSG_MSGCAST:
        {
            //qDebug() << "Client  LAYMSG_MSGCAST";
            if (ActSenLAYMSG_MSGCASTUnpack(m_nRecvMsg.pData,m_nRecvMsg.nDataLen))
            {
                // to Main Exchange Class do with the Data
                // 处理接收到的广播报文
                CE_VHFNodeManage::getInstance()->PackToSendRMTtoRSCMessageData(m_nRecvMsg.nSource,m_nRecvMsg.nReceive, \
                                                                               (unsigned char*)m_pMsgRecvData,m_pMsgRecvLen,true);

                strDesc = QString("%1=>LAYMSG_MSGCAST").arg(m_nRecvMsg.nSource);
            }
        }
        break;
    case LAYMSG_MSGONCE:
        {
            //qDebug() << "Client  LAYMSG_MSGONCE";
            if (ActSenLAYMSG_MSGONCEUnpack(m_nRecvMsg.pData,m_nRecvMsg.nDataLen))
            {
                // Add the Recall
                pCSCObjRecall msg(new CSCObjRecall);
                msg->nSource = m_nRecvMsg.nSource;
                msg->nSerial = m_pMsgRecvSn;
                m_nListRecall.push_back(msg);
                qDebug() << "CLIENT LAYMSG_MSGONCE msg.nSource " << msg->nSource << " msg.nSerial " << msg->nSerial;
                // to Main Exchange Class do with the Data
                // 处理接收到的报文
                CE_VHFNodeManage::getInstance()->PackToSendRMTtoRSCMessageData(m_nRecvMsg.nSource,m_nRecvMsg.nReceive,\
                                                                               (unsigned char*)m_pMsgRecvData,m_pMsgRecvLen,false);

                strDesc = QString("%1=>LAYMSG_MSGONCE").arg(m_nRecvMsg.nSource);

            }
        }
        break;
    case LAYMSG_MSGCALL:
        {
            //qDebug() << "Client  LAYMSG_MSGCALL";
            if (ActSenLAYMSG_MSGCALLUnpack(m_nRecvMsg.pData,m_nRecvMsg.nDataLen))
            {
                // to Main Exchange Class do with the data

                for (int i = 0; i < m_nRecvCallList.length(); ++i)
                {
                    CE_VHFNodeManage::getInstance()->RMTtoRSCMessageSerial(m_nRecvCallList[i]->nSource,m_nRecvCallList[i]->nSerial);
                }

                CE_VHFNodeManage::getInstance()->ReSetListCountNum();
                strDesc = QString("%1=>LAYMSG_MSGCALL").arg(m_nRecvMsg.nSource);
            }
        }
        break;
    default:
        break;
    }


    //////////////////////////////////////////////////////////////////////////
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
void CSC_01LayerClient::XTimeMaintenanceCircle()
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
        if (m_nTNotInChainCt >= m_nTNotInChainCtLmt)
        {
            CE_VHFNodeManage::getInstance()->VHFLayerChangeClientToHead();
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//
// Link Layer Control
//
//////////////////////////////////////////////////////////////////////////
// Link Layer Circle
// Main Control
void CSC_01LayerClient::LinkLayerMainCircle()
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
            //qDebug() << "Client  MOMENT_CIRCLE";
            m_nTOutCount++;

            if (m_bTCircleSend)
            {
                m_nTCircleDriftCount++;
                if (m_nTCircleDriftCount >= m_nTCircleDrift)
                {
                    ///////////////////////////////////////////////////////
                    // Send the Message
                    qDebug() << "Client MOMENT_CIRCLE id " << m_nCodeMe;
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
            //qDebug() << "Client  MOMENT_APPLY";
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
            //qDebug() << "Client  MOMENT_DRIFT";
            m_nTOutCount++;
            if (m_nTOutCount >= m_nChain.nLimitDrift*m_nTimeFactor)
            {
                m_bChainCircleFlag = false;
                m_nTNotInChainCt = -m_IsInChainWhereNum * 50;
                qDebug() << QString("Client m_nTNotInChainCt %1  MOMENT_DRIFT m_nTOutCount >= m_nChain.nLimitDrift*m_nTimeFactor").arg(m_nTNotInChainCt);

                m_nChain.Clear();		// Clear Chain
            }
        }
        break;
    default:
        break;
    }
}

// Send Message
void CSC_01LayerClient::LinkLayerComSendMemoryData()
{
    qDebug() << "LinkLayerComSendMemoryData";
    if (m_bSendOK)
    {
        qDebug() << "return LinkLayerComSendMemoryData  m_bSendOK";
        return;
    }

    ///////////////////////////////////////////////////////////////////
    // Add Send Message Data
    memset(m_pDSendData,0,PACK_LENGTHLIMIT);
    m_pDSendLen	 = 0;

    int nRecall = CE_VHFNodeManage::getInstance()->VHFLayerSendDataFromListWait(m_nDataMaxLen);
    if (nRecall == 1)
    {
        qDebug() << "Client nRecall";
        qDebug() << QString("%1<=LAYAPP_TEXT").arg(m_nCodeMe);
        //((CCE_VHFNodeDlg*)m_pMain)->VHFLayerReportControlMessage(LAYMSG_MSGONCE,strDesc);
    }

    // Still Online
    DataLayerMessageStateApply(LAYAPP_STILL,QString(""));	// use the space
    DataLayerSendMemoryGather(m_pDApplyData,m_pDApplyLen);

    qDebug() << QString("%1<=LAYAPP_STILL").arg(m_nCodeMe);
//    ((CCE_VHFNodeDlg*)m_pMain)->VHFLayerReportControlMessage(LAYMSG_STATE,strDesc);


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

        qDebug() << QString("%1<=MSG_RECALL").arg(m_nCodeMe);
//        ((CCE_VHFNodeDlg*)m_pMain)->VHFLayerReportControlMessage(LAYMSG_MSGCALL,strDesc);

    }

    // Send Data to the Layer
    m_bSendOK = ComSendOutData(m_pDSendData,m_pDSendLen);
    qDebug() << "Com Send Out Data";
}

void CSC_01LayerClient::LinkLayerComSendApplyData()
{
    // Send the Apply Moment Data
    qDebug() << "Client  LinkLayerComSendApplyData";
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
void CSC_01LayerClient::LinkLayerCircleMomentToBegin()
{
    m_nMoment	= MOMENT_BEGIN;
    m_nTOutCount = 0;

    LinkLayerCircleMomentToCircle();
}

// Layer Into Circle Moment
void CSC_01LayerClient::LinkLayerCircleMomentToCircle()
{
    m_nMoment	= MOMENT_CIRCLE;
    m_bChainCircleFlag	= true;
    m_nTOutCount	= 0;
}

// Layer Into Apply Moment
void CSC_01LayerClient::LinkLayerCircleMomentToApply()
{
    m_nMoment	= MOMENT_APPLY;
    m_nTOutCount	= 0;
}

// Layer Into Drift Moment
void CSC_01LayerClient::LinkLayerCircleMomentToDrift()
{
    m_nMoment	= MOMENT_DRIFT;
    m_nTOutCount	= 0;
}

// Do With the Out Time
void CSC_01LayerClient::LinkLayerChainDealWithOuttime(int& nid)
{
    qDebug() << "Client  LinkLayerChainDealWithOuttime";
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
void CSC_01LayerClient::LinkLayerChainDealWithRecvMsg(int& nid)
{
    qDebug() << "Client  LinkLayerChainDealWithRecvMsg";
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

/*	LinkLayerCircleMomentToApply();*/

}

void CSC_01LayerClient::RecordInChainPosition(CSCModelChain& nChain,int nId)
{
    for (int i = 0; i < nChain.nListMember.length(); ++i)
    {
        pCSCObjStage obj = nChain.nListMember[i];
        if (obj->id == nId)
        {
            m_IsInChainWhereNum = i;
            return;
        }
    }

    m_IsInChainWhereNum  = 0;
    return;
}

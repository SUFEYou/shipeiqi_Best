#include "RadioLink.h"
#include "LinkCommon.h"
#include "RadioLinkManage.h"
#include "config/ConfigLoader.h"
#include <QDateTime>
#include <QDebug>

#define MAXDATALENGTH 300

RadioLink::RadioLink()
{
    //////////////////////////////////////////////////////////////////////////
    // Link Layer Control  Parameter
    // Time Control :: Setting Value
    m_nTOutCount	= 0;		// Out Time
    m_nTApply		= 0;		// Save the Random Apply Moment's time
    m_nTCircleDrift	= 0;		// Set After the Stage Receive the Message, Drift to Send message Time
    m_nTCircleDriftCount = 0;	// Circle Drift Count
    m_bTCircleSend	= false;	// Is or Not Can Send the Message

    m_nMoment		= 0;		// Running Moment,Listen/Begin/Circle/Apply/Drift
    m_nDataMaxLen	= 0;

    // Own Stage Base Control Information
    m_bAvailable = false;	// Available
    m_nCodeMe	= 0;		// Stage Code
    m_nCodeVer	= 0;		// Version Code
    m_nChainIDNow	= 0;	// Now Chain ID
    m_nChainIDGo	= 0;	// Want to Enjoin the Chain
    m_bChainCircleFlag	= false;	// In or Out the Chain Communicate
    m_nTimeFactor = 1;		// Time Step Factor

    m_nMeState	= 0;			// Me's State,can or Not Send Message

    m_nSeatPrv	= 0;		// Previous	Send Message's Seat Number
    m_nSeatNow	= 0;		// Now Send Message's Seat Number

    m_bSendOK	= false;	// Is or Not Send Message Success
    m_bSendApplyCan = false;
    m_bMonitorAll	= false;		// Is or Not can Monitor Layer Message

    // Message Translate Parameter
    m_nSerial	= 0;			// Message's Serial
    //////////////////////////////////////////////////////////////////////////
    // Data Layer Base Parameter
    m_pDSendData	= new char[PACK_LENGTHLIMIT];	// Save the Data to Send to Layer
    m_pMsgRecvData	= new char[512];	// Save the Receive Data
    m_pMsgRecvLen	= 0;				// Receive Data's Length
    m_pMsgRecvSn	= 0;			// Receive Message's Serial

    m_pDSendLen		= 0;				// Send Data's Length
    m_pExData		= new char[512];	// Exchange Data Space
    m_pExDataLen	= 0;				// Exchange Data Space Length
    // LAYMSG_STATE :: State
    m_nRecvState	= 0;			// Apply State
    m_strStateDesc	= "";			// Apply Describe

    // Save Apply Data to Send to Layer
    m_pDApplyData	= new char[512];	// Apply Data
    m_pDApplyLen	= 0;

    m_nSendLater	= 0;

    //////////////////////////////////////////////////////////////////////////
    // Monitor the Member State
    m_nMonCode		= 0;                        // Monitoring State Code
    m_nMonRecvTime	= QTime::currentTime();     // Receive the Message Time
}

RadioLink::~RadioLink()
{
    if (m_pMsgRecvData)
    {
        delete []m_pMsgRecvData;
        m_pMsgRecvData = NULL;
    }

    if (m_pDSendData)
    {
        delete []m_pDSendData;
        m_pDSendData = NULL;
    }

    if (m_pExData)
    {
        delete []m_pExData;
        m_pExData = NULL;
    }

    if (m_pDApplyData)
    {
        delete []m_pDApplyData;
        m_pDApplyData = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////
// Set the class is or not Available
void RadioLink::SetAvailable(bool available)
{
    m_bAvailable = available;
}

bool RadioLink::GetAvailable()
{
    return m_bAvailable;
}

void RadioLink::setCodeVer(int x)
{
    m_nCodeVer = x;
}

void RadioLink::recvData(const char* pchar,const int nlength)
{
    m_recvDataArray.append(pchar, nlength);
    recvDataSubpackage();
    recvDataParse();
}

int RadioLink::DataLayerSendMemoryGatherJudge(bool bencrypt,const int nlength,int maxlength)
{
    int vhfpack = 0;
    if (bencrypt)
    {
        vhfpack = 12;
    }
    else
    {
        vhfpack = 10;
    }

    // Judge it is or Not can Add One Message head
    if (m_pDSendLen+vhfpack > maxlength)
    {
        return -1L;
    }

    // Out Length Limit
    if (m_pDSendLen+vhfpack+2+nlength > maxlength)
    {
        return 0L;
    }

    // Can Add to Send Memory
    return 1L;

}

// Encode the Data Layer Sentence
// Return Value:
//			-1 : No Space to Add Message Top
//			 0 : Out of Length Limit, Message is too Long
//			 1 : Still Have Free Space
void RadioLink::DataLayerSendMemoryGather(char* pchar,const int nlength)
{
    // Add the Sentence to this Send Array
    memcpy(m_pDSendData+m_pDSendLen,pchar,nlength);
    m_pDSendLen += nlength;
}

//广播报编码
void RadioLink::castMessageEncode(char* psource,int nlength,bool bencrypt,int nsource,int naim,char* pencode,int& enlen)
{
    ObjMsg msg;
    msg.nVersion = m_nCodeVer;
    msg.nSource	= nsource;
    msg.nReceive = naim;
    msg.bEncrypt = bencrypt;
    ActSenLAYMSG_MSGCASTPack(psource,nlength);

    memcpy(msg.pData,m_pExData,m_pExDataLen);
    msg.nDataLen = m_pExDataLen;

    DataLayerMessageEncode(msg,pencode,enlen);
}

//单发报编码
void RadioLink::onceMessageEncode(char* psource,int nlength,bool bencrypt,int nsource,int naim,char* pencode,int& enlen,int& serial)
{
    ObjMsg msg;
    msg.nVersion = m_nCodeVer;
    msg.nSource	= nsource;
    msg.nReceive = naim;
    msg.bEncrypt = bencrypt;
    ActSenLAYMSG_MSGONCEPack(psource,nlength,serial);

    memcpy(msg.pData,m_pExData,m_pExDataLen);
    msg.nDataLen = m_pExDataLen;

    DataLayerMessageEncode(msg,pencode,enlen);
}

//////////////////////////////////////////////////////////////////////////
// Pack Data into Layer Message
void RadioLink::DataLayerMessageEncode(ObjMsg& msg,char* pencode,int& enlen)
{
    // Encode the Data Layer Sentence
    char a;
    char tmp[4];

    // Begin Mark
    pencode[0] = 0xAA;

    // Version Number & Encrypt Mark
    a = msg.nVersion;
    a <<= 4;
    pencode[4] = a;
    a= msg.bEncrypt;
    a <<= 4;
    a >>= 4;
    pencode[4] += a;

    // Is or Not Encrypt
    if (!msg.bEncrypt)
    {
        enlen = 5;
    }
    else
    {
        enlen = 7;
        pencode[5] = 'B';		// Encrypt Key
        pencode[6] = 'E';
    }

    // Receive ID
    memset(tmp,0,4);
    memcpy(tmp,&msg.nReceive,sizeof(int));
    pencode[enlen] = tmp[1];
    pencode[enlen+1] = tmp[0];
    enlen += 2;

    // Source ID
    memset(tmp,0,4);
    memcpy(tmp,&msg.nSource,sizeof(int));
    pencode[enlen] = tmp[1];
    pencode[enlen+1] = tmp[0];
    enlen += 2;

    // Message Data
    memcpy(pencode+enlen,msg.pData,msg.nDataLen);
    enlen += msg.nDataLen;

    // End mark
    pencode[enlen] = 0xAA;

    // All Message pack Length
    enlen += 1;

    // Pack Length
    pencode[3] = enlen;

    // Encrypt the Data
    if (msg.bEncrypt)
    {
        ActEncrypt_KeyCreate(pencode+4,msg.nDataLen+6);
    }

    // CheckNum
    a = pencode[3];
    for (int i=4;i<enlen-1;i++)
    {
        a ^= pencode[i];
    }
    pencode[2] = a;

    // Get the 0xAA Empty Num
    ActEncrypt_CharClear(&pencode[2],enlen-3,pencode[1],0xAA);

}

void RadioLink::recvDataSubpackage()
{
    if (m_recvDataArray.length() < 10)//10 == 包头(1字节)+替换(1字节)+校验(1字节)+包长(1字节)+版本加密(1字节)+接收ID(2字节)+源ID(2字节)+包尾(1字节)
        return;
    else//数据分包
    {
        int  step = 0;
        int  contBegin = 0;
        int  contLen = 0;
        //删除数据长度
        int  removeLen = 0;
        for (int i = 0; i < m_recvDataArray.length(); ++i)
        {
            if (step == 0)//定位包头
            {
                if ((unsigned char)m_recvDataArray[i] == 0xAA)
                {
                    step = 1;
                    contBegin = i+1;
                    contLen = 0;
                }
            }
            else if (step == 1)//定位包尾
            {
                if ((unsigned char)m_recvDataArray[i] == 0xAA)
                {
                    if (contLen == 0)//包头丢失，包尾与下一包包头粘连情况（XX XX AA AA XX XX XX XX AA）
                    {
                        contBegin = i+1;
                        contLen = 0;
                        continue;
                    }
                    else if (contLen < 8)//包内数据长度小于8字节，数据长度错误
                    {
                        step = 0;
                        continue;
                    }
                    else
                    {
                        //将头和尾的0XAA都加入队列，解析时会用到
                        m_recvDataList.push_back(m_recvDataArray.mid(contBegin-1, contLen+2));
                        //更新已解码成功的数据长度
                        //removeLen = contBegin + contLen + 1;
                        removeLen = contBegin + contLen;
                        step = 0;
                        continue;
                    }
                }
                else
                {
                    ++contLen;
                }
            }

        }
        m_recvDataArray.remove(0, removeLen);
    }
}

void RadioLink::recvDataParse()
{
    while (!m_recvDataList.isEmpty())
    {
        m_nRecvMsg.Clear();
        int len = 0;

        QByteArray tmpArray = m_recvDataList.first();
        m_recvDataList.pop_front();

        char tmp[MAXDATALENGTH];
        memset(tmp, 0, MAXDATALENGTH);
        memcpy(tmp, tmpArray.data(), tmpArray.length());

//        printf("\n>>>>>>>>>>>>>>>>>>>>>> Recv Data >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

//        for(int i = 0; i < tmpArray.size(); ++i)
//            printf("%02x ", tmp[i]);

//        printf("\n>>>>>>>>>>>>>>>>>>>>>>> Recv Data >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
//        fflush(stdout);

        // Clear the 0xAA Empty Num
        ActEncrypt_CharAdd(tmp+2, tmpArray.length()-3, tmp[1]);
        // Check Number
        char a = tmp[3];
        for (int i = 4; i < tmpArray.length()-1; ++i)
        {
            a ^= tmp[i];
        }

        if (tmp[2] != a)
        {
            LOGW("In RadioLink::recvDataParse() CRC Err");

//            printf("\n>>>>>>>>>>>>>>>>>>>>>> CRC Err >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

//            for(int i = 0; i < tmpArray.size(); ++i)
//                printf("%02x ", tmp[i]);

//            printf("\n>>>>>>>>>>>>>>>>>>>>>>> CRC Err >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
//            fflush(stdout);

            continue;
        }
        // Message Length
        if (((unsigned char)tmp[3]) != tmpArray.length())
        {
            LOGW("In RadioLink::recvDataParse() Length Err");

//            printf("\n>>>>>>>>>>>>>>>>>>>>>> Length Err >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

//            for(int i = 0; i < tmpArray.size(); ++i)
//                printf("%02x ", tmp[i]);

//            printf("\n>>>>>>>>>>>>>>>>>>>>>>> Length Err >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
//            fflush(stdout);

            continue;
        }
        // Version & Encrypt
        m_nRecvMsg.nVersion = tmp[4] >> 4;
        m_nRecvMsg.bEncrypt = tmp[4] & 0x0F;

        if (m_nRecvMsg.bEncrypt)
        {
            len = 7;
            m_nRecvMsg.nDataLen = tmpArray.length()-12;
        }
        else
        {
            len = 5;
            m_nRecvMsg.nDataLen = tmpArray.length()-10;
        }

        // Decrypt
        if (m_nRecvMsg.bEncrypt)
        {
            ActEncrypt_KeyOpen(tmp+4, tmpArray.length()-6);
        }

        // Receive ID
        m_nRecvMsg.nReceive = (((unsigned char)tmp[len])<<8) | ((unsigned char)tmp[len+1]);
        len += 2;

        // Source ID
        m_nRecvMsg.nSource	= (((unsigned char)tmp[len])<<8) | ((unsigned char)tmp[len+1]);
        len += 2;

        // Data
        memcpy(m_nRecvMsg.pData, tmp+len, tmpArray.length()-1-len);
        recvDataAnalyze(m_nRecvMsg);
    }
}

// Do With the Step Touch
bool RadioLink::LinkLayerComStepTouch(int ncurid)
{
    int flag = 0;

    for (int i = 0; i < m_nChain.nListMember.length(); ++i)
    {
        pObjStage obj = m_nChain.nListMember[i];
        if (flag == 0)
        {
            if (obj->id == ncurid)
            {
                flag = 1;
                m_nSeatPrv = ncurid;
                m_nSeatNow = 0;
            }
        }
        else if (flag == 1 )
        {
            if (obj->state == LAYSTA_ONLINE)
            {
                m_nSeatNow = obj->id;
                return true;
            }
        }
    }
    if (flag == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}


//////////////////////////////////////////////////////////////////////////
// Link Layer Sentence
// LAYMSG_CONTROL :: Message to Control the Layer's Information
bool RadioLink::ActSenLAYMSG_CONTROLPack(ModelChain& chain)
{
    memset(m_pExData,0,512);
    m_pExDataLen = 0;

    // LAYMSG_CONTROL :: Message to Control the Layer's Information
    m_pExData[0] = LAYMSG_CONTROL;

    char a = 0;
    int f = 0;
    char tmp[4];

    // Chain ID
    f = chain.nChainId;
    memset(tmp,0,4);
    memcpy(tmp,&f,sizeof(int));
    m_pExData[1] = tmp[1];
    m_pExData[2] = tmp[0];

    // Limit Out
    m_pExData[3] = chain.nLimitOut;

    // Apply Time
    m_pExData[4] = chain.nLimitApply;

    // Drift Time
    m_pExData[5] = chain.nLimitDrift;

    // Member Information
    int num = 0;
    for (int i = 0; i < chain.nListMember.length(); ++i)
    {
        pObjStage stg = chain.nListMember[i];

        // Member ID
        memset(tmp,0,4);
        memcpy(tmp,&(stg->id),sizeof(int));
        m_pExData[7+num*3] = tmp[1];
        m_pExData[8+num*3] = tmp[0];

        // Control
        a = stg->state;
        a <<= 4;
        m_pExData[9+num*3] = a;

        // State
        a = stg->signal;
        a <<= 4;
        a >>= 4;
        m_pExData[9+num*3] += a;

        num++;
    }

    // Member Number
    m_pExData[6] = num;

    m_pExDataLen = num*3+7;

    return true;
}

bool RadioLink::ActSenLAYMSG_CONTROLUnpack(char* pchar,const int nlength)
{
    m_nRecvChain.Clear();

    // Chain ID
    m_nRecvChain.nChainId = (((unsigned char)pchar[1])<<8) | ((unsigned char)pchar[2]);

    // Out Time Limit
    m_nRecvChain.nLimitOut = pchar[3];

    // Apply Time Limit
    m_nRecvChain.nLimitApply = pchar[4];

    // Drift Limit Time
    m_nRecvChain.nLimitDrift = pchar[5];

    // Member Number
    m_nRecvChain.nMemNum = pchar[6];

    char a;
    // Member Information
    m_nRecvChain.nListMember.clear();

    for (int i=0; i<pchar[6]; i++)
    {
        if ((7+3*i) > nlength)
        {
            break;
        }

        pObjStage obj(new ObjStage);

        obj->id = (((unsigned char)pchar[7+3*i])<<8) | ((unsigned char)pchar[8+3*i]);

        a = pchar[9+3*i];
        a >>= 4;
        obj->state = a;
        a = pchar[9+3*i];
        a <<= 4;
        a >>= 4;
        obj->signal = a;
        m_nRecvChain.nListMember.push_back(obj);
    }

    return true;
}

// LAYMSG_STATE :: Layer State Control Message
bool RadioLink::ActSenLAYMSG_STATEPack(int nstate,QString strdesc)
{
    memset(m_pExData,0,512);
    m_pExDataLen = 0;

    m_pExData[0] = LAYMSG_STATE;

    // State
    m_pExData[1] = nstate;

    // Describe
    memcpy(m_pExData+2,strdesc.toStdString().c_str(),strdesc.length());
    m_pExDataLen = 2+strdesc.length();

    return true;
}

bool RadioLink::ActSenLAYMSG_STATEUnpack(char* pchar,const int nlength)
{
    // State
    m_nRecvState = pchar[1];

    // Describe
    char tmp[300];
    memset(tmp,0,300);
    memcpy(tmp,pchar+2,nlength-2);
    m_strStateDesc = QString(tmp);

    return true;
}

// LAYMSG_MSGCAST :: Broadcast Information
bool RadioLink::ActSenLAYMSG_MSGCASTPack(char* pdata,int nlength)
{
    memset(m_pExData,0,512);
    m_pExDataLen = 0;

    m_pExData[0] = LAYMSG_MSGCAST;

    memcpy(m_pExData+1,pdata,nlength);
    m_pExDataLen = 1+nlength;

    return true;
}

bool RadioLink::ActSenLAYMSG_MSGCASTUnpack(char* pchar,const int nlength)
{
    memset(m_pMsgRecvData,0,512);
    memcpy(m_pMsgRecvData,pchar+1,nlength-1);
    m_pMsgRecvLen = nlength-1;

    return true;
}

// LAYMSG_MSGONCE :: Layer's transmit Information
bool RadioLink::ActSenLAYMSG_MSGONCEPack(char* pdata,int nlength,int& nserial)
{
    memset(m_pExData,0,512);
    m_pExDataLen = 0;

    m_pExData[0] = LAYMSG_MSGONCE;

    uint32_t sysType = ConfigLoader::getInstance()->getSysType();
    if (sysType == 0X0A03)
    {
        // Serial
        if (nserial <= 0)
        {
            m_pExData[1] = 0;
            m_pExData[2] = 0;
        }
        else
        {
            m_pExData[1] = (nserial >> 8) &0xFF;
            m_pExData[2] = nserial &0xFF;
        }

        memcpy(m_pExData+3,pdata,nlength);
        m_pExDataLen = 3+nlength;
    }
    else if (sysType == 0X0A01)
    {
        if (nserial <= 0)
            m_pExData[1] = 0;
        else
            m_pExData[1] = nserial &0xFF;
        memcpy(m_pExData+2,pdata,nlength);
        m_pExDataLen = 2+nlength;
    }
    else
        return false;

    return true;
}

bool RadioLink::ActSenLAYMSG_MSGONCEUnpack(char* pchar,const int nlength)
{
    memset(m_pMsgRecvData,0,512);

    uint32_t sysType = ConfigLoader::getInstance()->getSysType();
    if (sysType == 0X0A03)
    {
        // Serial
        m_pMsgRecvSn = (((unsigned char)pchar[1])<<8) | ((unsigned char)pchar[2]);

        memcpy(m_pMsgRecvData,pchar+3,nlength-3);
        m_pMsgRecvLen = nlength-3;
    }
    else if (sysType == 0X0A01)
    {
        m_pMsgRecvSn = (unsigned char)pchar[1];

        memcpy(m_pMsgRecvData,pchar+2,nlength-2);
        m_pMsgRecvLen = nlength-2;
    }
    else
        return false;

    return true;
}

// LAYMSG_MSGCALL :: Message's Recall
bool RadioLink::ActSenLAYMSG_MSGCALLPack(int nmaxlen)
{
    int len;
    if (nmaxlen < 12)
    {
        len = 240;
    }
    else
    {
        len = nmaxlen;
    }

    memset(m_pExData,0,512);
    m_pExDataLen = 0;

    m_pExData[0] = LAYMSG_MSGCALL;
    m_pExDataLen = 1;

    if (m_nListRecall.isEmpty())
        return false;

    uint32_t sysType = ConfigLoader::getInstance()->getSysType();
    if (sysType == 0X0A03)
    {
        QList<pObjRecall>::iterator iter = m_nListRecall.begin();
        QList<pObjRecall>::iterator prvpos;
        while(iter != m_nListRecall.end())
        {
            prvpos = iter;
            pObjRecall msg = *iter;
            ++iter;
            if (m_pExDataLen+4 > len)
            {
                break;
            }
            else
            {
                m_pExData[m_pExDataLen]		= int(msg->nSource/256);
                m_pExData[m_pExDataLen+1]	= msg->nSource%256;
                m_pExData[m_pExDataLen+2]	=(msg->nSerial >> 8) & 0xFF;
                m_pExData[m_pExDataLen+3]   = msg->nSerial & 0xFF;

                m_pExDataLen += 4;
                m_nListRecall.erase(prvpos);
            }
        }
    }
    else if (sysType == 0X0A01)
    {
        QList<pObjRecall>::iterator iter = m_nListRecall.begin();
        QList<pObjRecall>::iterator prvpos;
        while(iter != m_nListRecall.end())
        {
            prvpos = iter;
            pObjRecall msg = *iter;
            ++iter;
            if (m_pExDataLen+3 > len)
            {
                break;
            }
            else
            {
                m_pExData[m_pExDataLen]		= int(msg->nSource/256);
                m_pExData[m_pExDataLen+1]	= msg->nSource%256;
                m_pExData[m_pExDataLen+2]   = msg->nSerial & 0xFF;

                m_pExDataLen += 3;
                m_nListRecall.erase(prvpos);
            }
        }
    }
    else
        return false;

    return true;
}

bool RadioLink::ActSenLAYMSG_MSGCALLUnpack(char* pchar,const int nlength)
{
    m_nRecvCallList.clear();

    int len = 1;

    uint32_t sysType = ConfigLoader::getInstance()->getSysType();
    if (sysType == 0X0A03)
    {
        while (1)
        {
            if (len+4 > nlength)
            {
                break;
            }

            pObjRecall msg(new ObjRecall);

            msg->nSource = (((unsigned char)pchar[len])<<8) | ((unsigned char)pchar[len+1]);
            msg->nSerial = (((unsigned char)pchar[len+2])<<8) | ((unsigned char)pchar[len+3]);
            len += 4;

            m_nRecvCallList.push_back(msg);
        }
    }
    else if (sysType == 0X0A01)
    {
        while (1)
        {
            if (len+3 > nlength)
            {
                break;
            }

            pObjRecall msg(new ObjRecall);

            msg->nSource = (((unsigned char)pchar[len])<<8) | ((unsigned char)pchar[len+1]);
            msg->nSerial = (unsigned char)pchar[len+2];
            len += 3;

            m_nRecvCallList.push_back(msg);
        }
    }
    else
        return false;

    return true;
}

//////////////////////////////////////////////////////////////////////////
// Apply to Enjoy or Leave the Chain
bool RadioLink::DataLayerMessageStateApply(int nstate,QString strdesc)
{
    // Judge the Apply Data is or not Null
    if (m_bSendApplyCan == false)
    {
        memset(m_pDApplyData,0,512);
        m_pDApplyLen = 0;

        int nMaxlen = 0;
        if (m_nChain.nLimitApply > 0)
        {
            nMaxlen = m_nChain.nLimitApply*m_nTimeFactor;
        }
        else
        {
            // Default Value is 5 seconds
            nMaxlen = 5*m_nTimeFactor;
        }
        m_nTApply = LinkLayerGetApplyTime(true,nMaxlen);
    }
    else
        return false;

    // Pack the State Message
    ActSenLAYMSG_STATEPack(nstate,strdesc);

    ObjMsg msg;
    msg.bEncrypt = false;
    msg.nDataLen = m_pExDataLen;
    memcpy(&msg.pData,m_pExData,m_pExDataLen);
    while (1)
    {
        if (m_nChain.nChainId > 0)
        {
            msg.nReceive = m_nChain.nChainId;
            break;
        }

        if (m_nChainIDNow > 0)
        {
            msg.nReceive = m_nChainIDNow;
            break;
        }

        if (m_nChainIDGo > 0)
        {
            msg.nReceive = m_nChainIDGo;
            break;
        }

        msg.nReceive = BROADCAST_ID;
        break;
    }

    msg.nVersion = m_nCodeVer;
    msg.nSource  = m_nCodeMe;
    DataLayerMessageEncode(msg,m_pExData,m_pExDataLen);

    // Add to Apply Send Data Space
    if (m_pDApplyLen+m_pExDataLen > 512)
    {
        return false;
    }

    memcpy(m_pDApplyData+m_pDApplyLen,m_pExData,m_pExDataLen);
    m_pDApplyLen += m_pExDataLen;

    return true;
}

//////////////////////////////////////////////////////////////////////////
// Data Interface
//////////////////////////////////////////////////////////////////////////
// Send Out Data to Terminal
bool RadioLink::ComSendOutData(char* pchar,int nlength)
{
    if (RadioLinkManage::getInstance()->PortCommSendOutData(pchar,nlength))
        return true;
    return false;
}

// Link Layer Get Apply Raid Time
int RadioLink::LinkLayerGetApplyTime(bool availabe,int maxvalue)
{
    if (!availabe)
    {
        return 99999;
    }

    srand(QDateTime::currentDateTimeUtc().toTime_t());
    int t1 = rand()%maxvalue;
    return t1;
}

//////////////////////////////////////////////////////////////////////////
// Encrypt the Message
// 创建密钥，生成加密报文
void RadioLink::ActEncrypt_KeyCreate(char* pchar,const int nlength)
{
    //生成密钥
    char t1,t2,t3,t4;
    t1=t2=t3=t4=0;

    t1=rand()%16;
    t2=rand()%16;
    t3=rand()%16;
    t4=t2^0x01;

    char j1=0,j2=0;
    j1=t1*16+t2;
    j2=t3*16+t4;
    j1=j1^'B';
    j2=j2^'S';
    //报头加密
    pchar[1]=j1;
    pchar[2]=j2;
    ////////////生成加密字符串/////////////
    char A1,A2,A3;
    A1=A2=A3=0;
    A1=(t1*16+t2)^'S';
    A2=(t3*16+t4)^'U';
    A3=(t3*16+t2)^'N';
    ActEncrypt_CodeCreate(A1,A2,A3);//得到加密码
    //加密报文
    for(int i=3;i<nlength;i++)
    {
        if(i<103)
            pchar[i]^=EncryptKey[i-3];
        else if(i>102&&i<203)
            pchar[i]^=EncryptKey[i-103];
        else if(i>202)
            pchar[i]^=EncryptKey[i-203];
    }
}

//解密数据
bool RadioLink::ActEncrypt_KeyOpen(char* pchar,const int nlength)
{
    char t1,t2,t3,t4;
    t1=t2=t3=t4=0;

    int j1,j2;
    j1=j2=0;
    j1=pchar[1]^'B';
    j2=pchar[2]^'S';

    t1=int(j1/16);
    t2=j1-t1*16;
    t3=int(j2/16);
    t4=j2-t3*16;


    if(t4 != (t2^0x01))
        return false;
    ////////////生成加密字符串/////////////
    char A1,A2,A3;
    A1=A2=A3=0;
    A1=(t1*16+t2)^'S';
    A2=(t3*16+t4)^'U';
    A3=(t3*16+t2)^'N';
    ActEncrypt_CodeCreate(A1,A2,A3);//得到加密码
    //解密数据
    for(int i=3;i<nlength;i++)
    {
        if(i<103)
            pchar[i]^=EncryptKey[i-3];
        else if(i>102&&i<203)
            pchar[i]^=EncryptKey[i-103];
        else if(i>202)
            pchar[i]^=EncryptKey[i-203];
    }
    return true;
}

//生成加密码
void RadioLink::ActEncrypt_CodeCreate(char A,char B,char C)
{
    memset(EncryptKey,0,100);
    bool b1,b2,b7,Bins,A8,B8,C8;
    b1=b2=b7=Bins=A8=B8=C8=0;
    int j=0;
    quint8 tmp=0;
    while(1)
    {
        EncryptKey[j]=A;
        if(j>98)
            break;
        EncryptKey[j+1]=B;
        EncryptKey[j+2]=C;
        j+=3;
        for(int i=0;i<24;i++)
        {
            tmp=A&128;
            if(tmp==128) b1=1;
            else b1=0;
            tmp=A&64;
            if(tmp==64)	b2=1;
            else b2=0;
            tmp=A&2;
            if(tmp==2) b7=1;
            else b7=0;
        /////取Bins值/////////
            C8=C&1;
            Bins=C8^b7;
            Bins^=b2;
            Bins^=b1;
        ///////移位A/B/C////////////
            A8=A&1;
            B8=B&1;
            A>>=1;
            if(Bins==1) A|=128;
            B>>=1;
            if(A8==1)	B|=128;
            C>>=1;
            if(B8==1)	C|=128;
        }
    }
}

// 去处数据中指定的字符fix
void RadioLink::ActEncrypt_CharClear(char* pchar,const int nlength,char& nEmp,const char fix)
{
#if 0
    //求取nEmp,去0x0A标记
    unsigned char tmp[256];
    unsigned char d=0;
    memset(tmp,0,256);
    for(int i=0;i<nlength;i++)
    {
        d = (unsigned char)pchar[i];
        tmp[d] = 1;
    }

    for(int i=255;i>=0;i--)
    {
        if(tmp[i]==0)
        {
            if (i != fix)
            {
                nEmp=i;
                break;
            }
        }
    }
    nEmp=256+fix-nEmp;

    //去除fix
    int a=0;
    for(int i=0;i<nlength;i++)
    {
        a=pchar[i]+nEmp;
        if(a>255)
            a-=256;
        pchar[i]=a;
    }
#else
    //求取nEmp,去0x0A标记
    quint8 tmp[256];
    unsigned char d=0;
    memset(tmp, 0, 256);
    for(int i = 0; i < nlength; ++i)
    {
        d = (unsigned char)pchar[i];
        tmp[d] = 1;
    }

    for(int i = 255; i >= 0; --i)
    {
        if(tmp[i] == 0)
        {
            if (i != fix)
            {
                nEmp = i;
                break;
            }
        }
    }
    nEmp = 256 + fix - nEmp;

    //去除fix
    int a=0;
    for(int i = 0; i < nlength; ++i)
    {
        a = pchar[i] + nEmp;
        if(a > 255)
            a -= 256;
        memset(&pchar[i], a, 1);
    }
#endif
}

// 恢复数据中的指定字符fix
void RadioLink::ActEncrypt_CharAdd(char* pchar,const int nlength,const char nEmp)
{
#if 0
    int a = 0;
    for(int i=0;i<nlength;i++)
    {
//        pchar[i]-=nEmp;
//        if(pchar[i]<0)
//            pchar[i] += char(256);
        a = pchar[i] - nEmp;
        if (a < 0)
            a += 256;
        pchar[i] = a;
    }
#else
    for(int i = 0; i < nlength; ++i)
    {
        int a = pchar[i]- nEmp;
        if ( a < 0)
        {
            a += 256;
        }
        memset(&pchar[i], a, 1);
    }
#endif
}

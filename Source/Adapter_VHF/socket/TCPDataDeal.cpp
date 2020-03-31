#include "TCPDataDeal.h"
#include <memory.h>
#include "VHFLayer/SC_01Layer.h"
#include "VHFLayer/CE_VHFNodeManage.h"
#include "socket/SocketManage.h"

TCPDataDeal* TCPDataDeal::m_TCPDataDeal = NULL;
QMutex TCPDataDeal::m_Mutex;

TCPDataDeal::TCPDataDeal()
{
    m_pCMDRecv = new unsigned char[RADIORTCCMDLEN];
    memset(m_pCMDRecv, 0, RADIORTCCMDLEN);
    m_nCMDRecvLen = 0;
    m_MRTPosData = new char[256];			//rodar of data
    memset(m_MRTPosData, 0, 256);
    m_MRTPosDataLen = 0;
    m_pBufSend		= new char[RADIORTCCMDLEN];		// 发送数据缓存
    memset(m_pBufSend, 0, RADIORTCCMDLEN);
    m_nBufSendLen	= 0;	// 发送数据缓存长度

    m_pCMDRecv			= new unsigned char[RADIORTCCMDLEN];
    m_nCMDRecvLen		= 0;

    m_nRSCID			= 0;
    m_strRSCDesc.clear();
    m_strRSCName.clear();
    m_nState			= 0;		// RSC State
    m_nGoing			= 0;		// 运行状态
    m_nLastTime			= 0;		// 最后更新时间

}

TCPDataDeal::~TCPDataDeal()
{
    if (m_TCPDataDeal != NULL)
        delete m_TCPDataDeal;
    if (m_MRTPosData != NULL)
        delete m_MRTPosData;
    if (m_pCMDRecv != NULL)
        delete m_pCMDRecv;
    if (m_pBufSend != NULL)
        delete m_pBufSend;
}

TCPDataDeal* TCPDataDeal::getInstance()
{
    if (m_TCPDataDeal == NULL)
    {
        QMutexLocker locker(&m_Mutex);
        if (m_TCPDataDeal == NULL)
        {
            m_TCPDataDeal = new TCPDataDeal;
        }
    }
    return m_TCPDataDeal;
}

void TCPDataDeal::recvTCPData(const char* data, const quint16 len)
{
    if (len > 0)
    {
        for (int i = 0; i < len; ++i)
        {
            onAnalyzeRemoteCTLData(data[i]);
        }
    }
}

// 发送数据
void TCPDataDeal::SendData(unsigned char* pData,int nLen)
{
    if (nLen <= 0)
    {
        SocketManage::getInstance()->tcpSendData(m_pCMDSend, m_nCMDSendLen);
    }
    else
    {
        SocketManage::getInstance()->tcpSendData(pData,nLen);
    }
}

//(1) 生成SLIP协议帧，数据长度不能为0
// Pack Slip Sentence Format
void TCPDataDeal::PackDataToSlipFormat(unsigned char* pSourceData,int nSourceLen)
{
    // m_pCMDSend 发送数据的缓存
    // m_nCMDSendLen 发送数据的长度
    memset(m_pCMDSend,0,RADIORTCCMDLEN);
    m_nCMDSendLen	= 0;

    // 开始标识
    m_pCMDSend[m_nCMDSendLen++]	= 0xC0;

    // 替换 0xC0 和 0xDB
    unsigned char tmp[RADIORTCCMDLEN];
    int  tmp_len = 0;
    memset(tmp,0,RADIORTCCMDLEN);

    for (int i = 0; i<nSourceLen; i++)
    {
        if (*(pSourceData+i) == 0xC0)
        {
            tmp[tmp_len++]	= 0xDB;
            tmp[tmp_len++]	= 0xDC;
        }
        else if (*(pSourceData+i) == 0xDB)
        {
            tmp[tmp_len++]	= 0xDB;
            tmp[tmp_len++]	= 0xDD;
        }
        else
        {
            tmp[tmp_len++]	= *(pSourceData+i);
        }
    }

    // 参数
    if (nSourceLen > 0)
    {
        memcpy(m_pCMDSend+m_nCMDSendLen,tmp,tmp_len);
        m_nCMDSendLen += tmp_len;

        // CRC校验
        unsigned char nCRC = m_pCMDSend[1];
        for(int i=2; i<m_nCMDSendLen; i++)
        {
            nCRC	^= m_pCMDSend[i];
        }

        unsigned char nCRCData = nCRC;
        nCRCData >>= 4;
        m_pCMDSend[m_nCMDSendLen++]	= nCRCData;
        nCRCData	= nCRC;
        nCRCData <<= 4;
        nCRCData >>= 4;
        m_pCMDSend[m_nCMDSendLen++]	= nCRCData;
    }
    else
    {
        // Check Number
        m_pCMDSend[m_nCMDSendLen++]	= 0x00;
        m_pCMDSend[m_nCMDSendLen++]	= 0x00;
    }

    // 结束标识
    m_pCMDSend[m_nCMDSendLen++]	= 0xC0;
}

//(2) 从串行数据中，获取SLIP封装数据
// Get State Information from Recall Data
void TCPDataDeal::onAnalyzeRemoteCTLData(const unsigned char nChar)
{
    // m_pCMDRecv:接收数据的缓存
    // m_nCMDRecvLen：接收数据的长度
    // Port Feed Back time is zero

    // Analyze Receive Data
    if (nChar == 0xC0)
    {
        if (m_nCMDRecvLen > 2)
        {
            // Change Format
            if(onAnalyzeSentenceToSlipFormat(m_pCMDRecv,m_nCMDRecvLen))
            {
                // Analyze Slip Sentence
                analyzeNetMsg(m_pCMDRecv,m_nCMDRecvLen);
            }
        }
        memset(m_pCMDRecv,0,RADIORTCCMDLEN);
        m_nCMDRecvLen = 0;
    }
    else
    {
        m_pCMDRecv[m_nCMDRecvLen++] = nChar;
    }

    // Error Sentence
    if (m_nCMDRecvLen > 1000)
    {
        memset(m_pCMDRecv,0,RADIORTCCMDLEN);
        m_nCMDRecvLen = 0;
    }
}

//(3) 从SLIP封装中还原数据
// Get Slip Sentence
bool TCPDataDeal::onAnalyzeSentenceToSlipFormat(unsigned char* pChar, quint16& nLen)
{
    //bool bSt = false;
    int nS1 = 0;

    // Get Check Number
    char nCRC = *(pChar);
    for (int i = 1; i<nLen-2; i++)
    {
        nCRC ^= *(pChar+i);
    }

    // Get Now Check Number
    char nCRCGet = *(pChar+nLen-2);
    nCRCGet	<<= 4;
    nCRCGet += *(pChar+nLen-1);

    if (nCRC != nCRCGet)
        return FALSE;

    unsigned char nTmp[RADIORTCCMDLEN];
    quint16 nLenTmp = nLen;
    memset(nTmp, 0, RADIORTCCMDLEN);
    memcpy(nTmp, pChar, nLenTmp);

    for (int i = 0; i<nLenTmp; i++)
    {
        if (nTmp[i] == 0xDB)
        {
            if (nTmp[i+1] == 0xDC)
            {
                *(pChar+nS1) = 0xC0;
                nS1++;
                i++;
            }
            else if (nTmp[i+1] == 0xDD)
            {
                *(pChar+nS1) = 0xDB;
                nS1++;
                i++;
            }
            else
            {
                *(pChar+nS1) = nTmp[i];
                nS1++;
            }
        }
        else
        {
            *(pChar+nS1) = nTmp[i];
            nS1++;
        }
    }
    nLen = nS1;
    return true;
}

void TCPDataDeal::analyzeNetMsg(unsigned char* pData,const int nLen)
{
    if (nLen <= static_cast<int>(sizeof(NET_MSG_HEADER)))
    {
        return;
    }

    int nCurLen = 0;
    NET_MSG_HEADER	m_sRecvHead;			// Receive header STRUCT
    memcpy(&m_sRecvHead, pData, sizeof(NET_MSG_HEADER));
    nCurLen += sizeof(NET_MSG_HEADER);

    if (m_sRecvHead.MessageModel != A01VLN_MSGMODEL_STATION_EX)
    {
        return;
    }
    switch(m_sRecvHead.MessageType)
    {
    case VLNMSG_ACC_STATE:					// 区域中心（ACC）状态
        break;
    case VLNMSG_ACC_BASEINFO:				// 区域中心（ACC）基本信息
        break;
    case VLNMSG_RSC_STATE:					// 基站控制台（RSC）状态
        break;
    case VLNMSG_RSC_BASEINFO:				// 基站控制台（ACC）基本信息
        break;
    case VLNMSG_MRT_STATE:					// 移动终端（MRT）状态
        break;
    case VLNMSG_MRT_BASEINFO:				// 移动终端（MRT）基本信息
        break;
    case VLNMSG_APT_STATE:					// 应用终端（APT）状态
        break;
    case VLNMSG_APT_BASEINFO:				// 应用终端（APT）基本信息
        break;
    case VLNMSG_MRT_POSITION:				// 移动台位置报文
        {
            if (nLen < 256)
            {
                NET_MSGEX_TEXT sText;
                memcpy(&sText, pData+nCurLen, sizeof(NET_MSGEX_TEXT));
                nCurLen += sizeof(NET_MSGEX_TEXT);

                memset(m_MRTPosData,0,256);
                memcpy(m_MRTPosData,pData+nCurLen-1,sText.TextLength);
                m_MRTPosDataLen = sText.TextLength;
            }
        }
        break;
    case VLNMSG_MSGEX_TEXT:					// 二进制短报文信息
        {
            // 报文信息
            // 解析，加入发送报文队列
            NET_MSGEX_TEXT sText;
            memcpy(&sText, pData+nCurLen, sizeof(NET_MSGEX_TEXT));
            nCurLen += sizeof(NET_MSGEX_TEXT);

            CE_VHFNodeManage::getInstance()->ACCtoRSCMessageData(sText.SendID, sText.RecvID, (char*)(pData+nCurLen-1), sText.TextLength, sText.Encrypt, sText.Degree, sText.Serial);

        }
        break;
    case VLNMSG_MSGEX_DELETED:
        {

            NET_MSGEX_MSGDELETED dObject;
            memcpy(&dObject, pData+nCurLen, sizeof(NET_MSGEX_MSGDELETED));

            nCurLen += sizeof(NET_MSGEX_MSGDELETED);

            CE_VHFNodeManage::getInstance()->DeleteACCtoRSCMessageData(dObject.SendID, dObject.SerialBegin, dObject.SerialEnd);
        }
        break;
    case VLNMSG_MSGEX_RECALLCODE:			// 二进制短报文回馈序号
        break;
    case VLNMSG_RSC_UPDATEMRT:				// 基站控制台上报移动终端状态:
        break;
    default:
        break;
    }
}

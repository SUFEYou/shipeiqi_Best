#include "TCPDataDecoder.h"
#include <memory.h>
#include "common.h"

TCPDataDecoder* TCPDataDecoder::m_TCPDataDecoder = NULL;
QMutex TCPDataDecoder::m_Mutex;

TCPDataDecoder::TCPDataDecoder()
{
    m_pCMDRecv = new char[RADIORTCCMDLEN];
    memset(m_pCMDRecv, 0, RADIORTCCMDLEN);
    m_nCMDRecvLen = 0;
    m_MRTPosData = new char[256];			//rodar of data
    memset(m_MRTPosData, 0, 256);
    m_MRTPosDataLen = 0;
}

TCPDataDecoder::~TCPDataDecoder()
{
    if (m_TCPDataDecoder != NULL)
        delete m_TCPDataDecoder;
    if (m_MRTPosData != NULL)
        delete m_MRTPosData;
    if (m_pCMDRecv != NULL)
        delete m_pCMDRecv;
}

TCPDataDecoder* TCPDataDecoder::getInstance()
{
    if (m_TCPDataDecoder == NULL)
    {
        QMutexLocker locker(&m_Mutex);
        if (m_TCPDataDecoder == NULL)
        {
            m_TCPDataDecoder = new TCPDataDecoder;
        }
    }
    return m_TCPDataDecoder;
}

void TCPDataDecoder::recvTCPData(const char* data, const quint16 len)
{
    if (len > 0)
    {
        for (int i = 0; i < len; ++i)
        {
            onAnalyzeRemoteCTLData(data[i]);
        }
    }
}

//从网络报文数据中，获取SLIP封装数据
void TCPDataDecoder::onAnalyzeRemoteCTLData(const char nChar)
{
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

//从SLIP封装中还原数据
bool TCPDataDecoder::onAnalyzeSentenceToSlipFormat(char* pChar, quint16& nLen)
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

    char nTmp[RADIORTCCMDLEN];
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

void TCPDataDecoder::analyzeNetMsg(const char* pData,const int nLen)
{
    if (nLen <= sizeof(NET_MSG_HEADER))
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

            //ACCtoRSCMessageData(sText.SendID,sText.RecvID,pData+nCurLen-1,sText.TextLength,sText.Encrypt,sText.Degree,sText.Serial);

        }
        break;
    case VLNMSG_MSGEX_DELETED:
        {

            NET_MSGEX_MSGDELETED dObject;
            memcpy(&dObject, pData+nCurLen, sizeof(NET_MSGEX_MSGDELETED));

            nCurLen += sizeof(NET_MSGEX_MSGDELETED);

            //DeleteACCtoRSCMessageData(dObject.SendID,dObject.SerialBegin,dObject.SerialEnd);
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

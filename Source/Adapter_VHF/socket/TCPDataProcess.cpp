#include "TCPDataProcess.h"
#include "socket/SocketManage.h"
#include "RadioLink/RadioLinkManage.h"
#include "config/ConfigLoader.h"
#include <time.h>
#include <QDebug>
#include "log/log4z.h"

using namespace zsummer::log4z;

TCPDataProcess* TCPDataProcess:: m_instance = NULL;
QMutex TCPDataProcess::m_mutex;

#define MAXDATALENGTH 1500

TCPDataProcess::TCPDataProcess()
{
    m_byteArray.clear();
    m_contentList.clear();

    m_MRTPosData = new char[256];			//rodar of data
    memset(m_MRTPosData, 0, 256);
    m_MRTPosDataLen = 0;
    //暂时设置为固定值
    m_nRSCID = 10001;
}

TCPDataProcess::~TCPDataProcess()
{
    if (m_MRTPosData != NULL)
    {
        delete []m_MRTPosData;
        m_MRTPosData = NULL;
    }
}

TCPDataProcess* TCPDataProcess::getInstance()
{
    if (m_instance == NULL)
    {
        QMutexLocker locker(&m_mutex);
        if (m_instance == NULL)
        {
            m_instance = new TCPDataProcess;
        }
    }
    return m_instance;
}

void TCPDataProcess::recvData(const char* data, const quint16 len)
{
    m_byteArray.append(data, len);
    TCPDataSubpackage();
    TCPDataParse();
}

void TCPDataProcess::packageAndSendData(const char* data, const quint16 len)
{
    if (len <= 0)
        return;
    char send_data[MAXDATALENGTH];
    memset(send_data, 0, MAXDATALENGTH);
    quint16 send_len	= 0;
    // 开始标识
    send_data[send_len++] = 0XC0;
    //替换0XC0，0XDB
    char tmp[MAXDATALENGTH];
    memset(tmp, 0, MAXDATALENGTH);
    quint16  tmplen = 0;
    for (int i = 0; i < len; ++i)
    {
        if ((unsigned char)data[i] == 0xC0)
        {
            tmp[tmplen++]	= 0xDB;
            tmp[tmplen++]	= 0xDC;
        }
        else if ((unsigned char)data[i] == 0xDB)
        {
            tmp[tmplen++]	= 0xDB;
            tmp[tmplen++]	= 0xDD;
        }
        else
        {
            tmp[tmplen++]	= data[i];
        }
    }
    //数据域
    memcpy(send_data+send_len, tmp, tmplen);
    send_len += tmplen;
    unsigned char nCRCData = CRCVerify(send_data+1, send_len-1);
    //CRC校验
    send_data[send_len++] = nCRCData >> 4;
    send_data[send_len++] = nCRCData & 0X0F;
    //结束标识
    send_data[send_len++] = 0XC0;
    sendData(send_data, send_len);
}

void TCPDataProcess::sendData(const char* pData,const quint16 nLen)
{
    SocketManage::getInstance()->getTcpClient()->sendData(pData,nLen);
    LOGD(QString("In TCPDataProcess::sendData, Send Data Len: %1").arg(nLen).toStdString().c_str());
}

void TCPDataProcess::TCPDataSubpackage()
{
    if (m_byteArray.length() < 3)//3 == 包头（1字节）+ 校验（2字节） + 包尾（1字节）
        return;
    else//数据分包
    {
        int  step = 0;
        int  contBegin = 0;
        int  contLen = 0;
        //删除数据长度
        int  removeLen = 0;
        for (int i = 0; i < m_byteArray.length(); ++i)
        {
            if (step == 0)//定位包头
            {
                if ((unsigned char)m_byteArray[i] == 0xC0)
                {
                    step = 1;
                    contBegin = i+1;
                    contLen = 0;
                }
            }
            else if (step == 1)//定位包尾
            {
                if ((unsigned char)m_byteArray[i] == 0xC0)
                {
                    if (contLen == 0)//包头丢失，包尾与下一包包头粘连情况（XX XX C0 C0 XX XX XX XX C0）
                    {
                        contBegin = i+1;
                        contLen = 0;
                        continue;
                    }
                    else if (contLen < 2)//包内数据长度小于校验字节长度2字节，数据长度错误
                    {
                        step = 0;
                        continue;
                    }
                    else
                    {
                        m_contentList.push_back(m_byteArray.mid(contBegin, contLen));
                        //更新已解码成功的数据长度
                        removeLen = contBegin + contLen + 1;
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
        m_byteArray.remove(0, removeLen);
    }
}

void TCPDataProcess::TCPDataParse()
{
    //解析包内容
    while (!m_contentList.isEmpty())
    {
        QByteArray tmpArray = m_contentList.first();
        m_contentList.pop_front();

        if (tmpArray.length() < 2 || tmpArray.length() > MAXDATALENGTH)
        {
            LOGD("In TCPDataParse(), Recv TCP Data Len Err, Too Short or Too Long");
            continue;
        }

        char tmp[MAXDATALENGTH];
        memset(tmp, 0, MAXDATALENGTH);
        memcpy(tmp, tmpArray.data(), tmpArray.length());

        char nCRC = CRCVerify(tmp, tmpArray.length()-2);
        char nCRCGet = (tmp[tmpArray.length()-2] << 4) + tmp[tmpArray.length()-1];

        if (nCRC != nCRCGet)
        {
            qDebug() << "Decoder Recv TCP Data CRC Err!";
        }
        else
        {
            char  state[MAXDATALENGTH];
            memset(state, 0, MAXDATALENGTH);
            int   stateLen = 0;
            for (int m = 0;  m < tmpArray.length()-2; ++m)
            {
                if ((unsigned char)tmp[m] != 0xDB)
                {
                    state[stateLen] = tmp[m];
                    ++stateLen;
                }
                else
                {
                    if ((unsigned char)tmp[m+1] == 0xDC)
                    {
                        state[stateLen] = 0xC0;
                        ++stateLen;
                        ++m;
                    }
                    else if ((unsigned char)tmp[m+1] == 0xDD)
                    {
                        state[stateLen] = 0xDB;
                        ++stateLen;
                        ++m;
                    }
                    else
                    {
                        state[stateLen] = tmp[m];
                        ++stateLen;
                    }
                }
            }

            //解包到正确数据，发出该数据
            if (stateLen > 0)
                analyzeNetMsg(state, stateLen);
        }
    }
}

char TCPDataProcess::CRCVerify(const char* data, const quint16 len)
{
    if (len <= 0)
        return 0;
    char crc = data[0];
    for (int i = 1; i < len; ++i)
        crc ^= data[i];
    return crc;
}

void TCPDataProcess::analyzeNetMsg(char* pData,const int nLen)
{
    if (nLen <= (int)sizeof(NET_MSG_HEADER))
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
                RadioLinkManage::getInstance()->ACCtoRSCPosData(m_MRTPosData, m_MRTPosDataLen);
                LOGD("In TCPDataProcess::analyzeNetMsg, Recv VLNMSG_MRT_POSITION 移动台位置报文");
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
            //存储类ObjMsg内定义数据最大字节为512字节
            if (sText.TextLength > 512)
                break;
            RadioLinkManage::getInstance()->ACCtoRSCMessageData(sText.SendID, sText.RecvID, pData+nCurLen-1, sText.TextLength, sText.Encrypt, sText.Degree, sText.Serial);
            LOGD("In TCPDataProcess::analyzeNetMsg, Recv VLNMSG_MSGEX_TEXT 二进制短报文信息");
        }
        break;
    case VLNMSG_MSGEX_DELETED:
        {

            NET_MSGEX_MSGDELETED dObject;
            memcpy(&dObject, pData+nCurLen, sizeof(NET_MSGEX_MSGDELETED));

            nCurLen += sizeof(NET_MSGEX_MSGDELETED);

            RadioLinkManage::getInstance()->DeleteACCtoRSCMessageData(dObject.SendID, dObject.SerialBegin, dObject.SerialEnd);
            LOGD("In TCPDataProcess::analyzeNetMsg, Recv VLNMSG_MSGEX_DELETED 删除报文");
        }
        break;
    case VLNMSG_MSGEX_SETLINKMODE:
    {
        // 设置链路模式 对应设置项中，0:自动，链路自动切换主副台 1:强制切换链路为主台 2:强制切换链路为副台
        char modeType = pData[nCurLen];
        if (modeType == 1)
        {
            RadioLinkManage::getInstance()->changeClientToMaster();
        }
        else if (modeType == 2)
        {
            RadioLinkManage::getInstance()->changeMasterToClient();
        }
        LOGD("In TCPDataProcess::analyzeNetMsg, Recv VLNMSG_MSGEX_SETLINKMODE 设置链路模式");
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

//////////////////////////////////////////////////////////////////////////
// 上报Client状态基本信息
//////////////////////////////////////////////////////////////////////////
// 上报基本状态
void TCPDataProcess::RSCtoACCUpdateBaseInfo()
{
    char send_data[MAXDATALENGTH];
    memset(send_data, 0, MAXDATALENGTH);
    quint16 send_len = sizeof(NET_MSG_HEADER);

    NET_RSC_BASEINFO sBase;
    memset(&sBase,0,sizeof(NET_RSC_BASEINFO));
    sBase.RSCID	= m_nRSCID;
    memcpy(sBase.Name, m_strRSCName.toStdString().c_str(), m_strRSCName.length());
    memcpy(sBase.Describe, m_strRSCDesc.toStdString().c_str(), m_strRSCDesc.length());

    memcpy(send_data+send_len, &sBase, sizeof(NET_RSC_BASEINFO));
    send_len += sizeof(NET_RSC_BASEINFO);

    // Header Information
    time_t ltime;
    time(&ltime);
    NET_MSG_HEADER sendHead;
    sendHead.ProgramType = ConfigLoader::getInstance()->getProgramType();
    sendHead.ProgramID   = ConfigLoader::getInstance()->getProgramID();
    sendHead.MessageLen		= send_len;
    sendHead.MessageSerial	= (unsigned long)(ltime);
    sendHead.MessageType		= VLNMSG_RSC_BASEINFO;
    memcpy(send_data, &sendHead, sizeof(NET_MSG_HEADER));

    packageAndSendData(send_data, send_len);
}

// 上报状态信息
void TCPDataProcess::RSCtoACCUpdateStateInfo()
{
    char send_data[MAXDATALENGTH];
    memset(send_data, 0, MAXDATALENGTH);
    quint16 send_len = sizeof(NET_MSG_HEADER);

    time_t ltime;
    time(&ltime);

    NET_RSC_STATE sState;
    memset(&sState,0,sizeof(NET_RSC_STATE));
    sState.RSCID	= m_nRSCID;
    sState.Going	= m_nGoing;
    sState.State	= m_nState;

    memcpy(send_data+send_len, &sState, sizeof(NET_RSC_STATE));
    send_len += sizeof(NET_RSC_STATE);

    // Header Information
    NET_MSG_HEADER sendHead;
    sendHead.ProgramType = ConfigLoader::getInstance()->getProgramType();
    sendHead.ProgramID   = ConfigLoader::getInstance()->getProgramID();
    sendHead.MessageLen = send_len;
    sendHead.MessageSerial = (unsigned long)(ltime);
    sendHead.MessageType = VLNMSG_RSC_STATE;
    memcpy(send_data, &sendHead, sizeof(NET_MSG_HEADER));
    packageAndSendData(send_data, send_len);
    LOGD("In TCPDataProcess::RSCtoACCUpdateStateInfo()!");
}

int TCPDataProcess::getRSCID()
{
    return m_nRSCID;
}

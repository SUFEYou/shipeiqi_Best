#ifndef TCPDATADEAL_H
#define TCPDATADEAL_H

#include <QMutex>
#include <QSharedPointer>
#include <QList>
#include "common.h"

class CSCRSC_ObjVHFMsg;

typedef QSharedPointer<CSCRSC_ObjVHFMsg> VHFMsg;

class TCPDataDeal
{
public:
    static TCPDataDeal* getInstance();
    void recvTCPData(const char* data, const quint16 len);

    //////////////////////////////////////////////////////////////////////////
    // 报文处理内容，如打包/解包
    //////////////////////////////////////////////////////////////////////////
    //(1) 生成SLIP协议帧，数据长度不能为0
    // Pack Slip Sentence Format
    void PackDataToSlipFormat(unsigned char* pSourceData,int nSourceLen);

    // 发送数据
    void SendData(unsigned char* pData = NULL,int nLen = 0);

private:
    TCPDataDeal();
    ~TCPDataDeal();
    //(2) 从串行数据中，获取SLIP封装数据
    // Get State Information from Recall Data
    void onAnalyzeRemoteCTLData(const unsigned char nChar);
    //(3) 从SLIP封装中还原数据
    // Get Slip Sentence
    bool onAnalyzeSentenceToSlipFormat(unsigned  char* pChar, quint16& nLen);
    void analyzeNetMsg(unsigned char* pData,const int nLen);
    void ACCtoRSCMessageData(const int nSendID, const int nRecvID, char* pChar,const int nLen, bool bEncrypt,int nDegree, const int nSerial);
    bool DeleteACCtoRSCMessageData(const int nSendID, const int nSerialBegin, const int nSerialEnd);

public:
    //////////////////////////////////////////////////////////////////////////
    // RSC的基本信息
    //////////////////////////////////////////////////////////////////////////
    int                             m_nRSCID;			// RSC ID
    QString                         m_strRSCName;		// 名称
    QString                         m_strRSCDesc;		// 描述
    int                             m_nState;			// RSC State
    int                             m_nGoing;			// 运行状态
    int                             m_nLastTime;		// 最后更新时间

    unsigned char*                  m_pCMDSend;			// 发送指令缓存
    int                             m_nCMDSendLen;		// 发送指令长度

private:
    static TCPDataDeal*             m_TCPDataDeal;
    static QMutex                   m_Mutex;

    unsigned char*                  m_pCMDRecv;             // 接收指令缓存
    quint16                         m_nCMDRecvLen;          // 接收指令长度

    // Send Messge Buffer
    NET_MSG_HEADER                  m_sSendHead;			// Send header STRUCT
    char*                           m_pBufSend;				// 发送数据缓存
    int                             m_nBufSendLen;			// 发送数据缓存长度

    char*                           m_MRTPosData;
    quint16                         m_MRTPosDataLen;

    QList<VHFMsg>                   m_lVHFMsgList;
};

#endif // TCPDATADEAL_H

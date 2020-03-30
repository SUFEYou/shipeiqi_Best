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

private:
    TCPDataDeal();
    ~TCPDataDeal();
    void onAnalyzeRemoteCTLData(const unsigned char nChar);
    bool onAnalyzeSentenceToSlipFormat(unsigned  char* pChar, quint16& nLen);
    void analyzeNetMsg(unsigned char* pData,const int nLen);
    void ACCtoRSCMessageData(const int nSendID, const int nRecvID, char* pChar,const int nLen, bool bEncrypt,int nDegree, const int nSerial);
    bool DeleteACCtoRSCMessageData(const int nSendID, const int nSerialBegin, const int nSerialEnd);

private:
    static TCPDataDeal*             m_TCPDataDeal;
    static QMutex                   m_Mutex;

    unsigned char*                           m_pCMDRecv;             // 接收指令缓存
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

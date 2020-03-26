#ifndef TCPDATADECODER_H
#define TCPDATADECODER_H

#include <QMutex>
#include <QSharedPointer>
#include <QList>

class CSCRSC_ObjVHFMsg;

typedef QSharedPointer<CSCRSC_ObjVHFMsg> VHFMsg;

class TCPDataDecoder
{
public:
    static TCPDataDecoder* getInstance();
    void recvTCPData(const char* data, const quint16 len);

private:
    TCPDataDecoder();
    ~TCPDataDecoder();
    void onAnalyzeRemoteCTLData(const char nChar);
    bool onAnalyzeSentenceToSlipFormat(char* pChar, quint16& nLen);
    void analyzeNetMsg(char* pData,const int nLen);
    void ACCtoRSCMessageData(const int nSendID, const int nRecvID, char* pChar,const int nLen, bool bEncrypt,int nDegree, const int nSerial);
    bool DeleteACCtoRSCMessageData(const int nSendID, const int nSerialBegin, const int nSerialEnd);

private:
    static TCPDataDecoder*          m_TCPDataDecoder;
    static QMutex                   m_Mutex;
    char*                           m_pCMDRecv;			// 接收指令缓存
    quint16                         m_nCMDRecvLen;		// 接收指令长度
    char*                           m_MRTPosData;
    quint16                         m_MRTPosDataLen;

    QList<VHFMsg>                   m_lVHFMsgList;
};

#endif // TCPDATADECODER_H

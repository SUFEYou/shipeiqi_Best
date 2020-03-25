#ifndef TCPDATADECODER_H
#define TCPDATADECODER_H

#include <QMutex>

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
    void analyzeNetMsg(const char* pData,const int nLen);

private:
    static TCPDataDecoder*          m_TCPDataDecoder;
    static QMutex                   m_Mutex;
    char*                           m_pCMDRecv;			// 接收指令缓存
    quint16                         m_nCMDRecvLen;		// 接收指令长度
    char*                           m_MRTPosData;
    quint16                         m_MRTPosDataLen;
};

#endif // TCPDATADECODER_H

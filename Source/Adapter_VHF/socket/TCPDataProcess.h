#ifndef TCPDATAPROCESS_H
#define TCPDATAPROCESS_H

#include <QMutex>
#include <QList>
#include <QByteArray>
#include <QString>
#include "RadioLink/LinkCommon.h"

class TCPDataProcess
{
public:
    static TCPDataProcess* getInstance();
    void recvData(const char* data, const quint16 len);
    void packageAndSendData(const char* data, const quint16 len);

    //////////////////////////////////////////////////////////////////////////
    // 上报Client状态基本信息
    //////////////////////////////////////////////////////////////////////////
    // 上报基本状态
    void RSCtoACCUpdateBaseInfo();
    // 上报状态信息
    void RSCtoACCUpdateStateInfo();

    int getRSCID();

private:
    TCPDataProcess();
    ~TCPDataProcess();

    // 发送数据
    void sendData(const char* pData,const quint16 nLen);
    void TCPDataSubpackage();
    void TCPDataParse();
    void analyzeNetMsg(char* pData,const int nLen);
    char CRCVerify(const char* data, const quint16 len);

private:
    static TCPDataProcess*          m_instance;
    static QMutex                   m_mutex;
    QByteArray                      m_byteArray;
    QList<QByteArray>               m_contentList;

    char*                           m_MRTPosData;
    quint16                         m_MRTPosDataLen;

    //////////////////////////////////////////////////////////////////////////
    // RSC的基本信息
    //////////////////////////////////////////////////////////////////////////
    int                             m_nRSCID;			// RSC ID
    QString                         m_strRSCName;		// 名称
    QString                         m_strRSCDesc;		// 描述
    int                             m_nState;			// RSC State
    int                             m_nGoing;			// 运行状态
    int                             m_nLastTime;		// 最后更新时间
};

#endif // TCPDATAPROCESS_H

#ifndef CE_VHFNODEMANAGE_H
#define CE_VHFNODEMANAGE_H

#include <QObject>
#include <QMutex>
#include "common.h"

#define RADIORTCCMDLENGTH 2048

class CSC_01LayerHead;
class CSC_01LayerClient;
class QTimer;

class CE_VHFNodeManage : public QObject
{
    Q_OBJECT
public:
    static CE_VHFNodeManage* getInstance();
    void init();


    // 向上反馈 ==>>
    // 上报当前接收到的报文
    void PackToSendRMTtoRSCMessageData(const int nSendID,const int nRecvID,unsigned char* pChar,const int nLen,bool bEncrypt);
    // 上报用户报文发送完成
    void PackToSendRMTtoRSCMessageSerial(const int nSendID,const int nSerial);
    // 上报当前链表状态
    void RSCtoACCChainState();


    //////////////////////////////////////////////////////////////////////////
    // VHF Layer 操作
    //////////////////////////////////////////////////////////////////////////
    // <==接收到链锯回馈的处理
    bool RMTtoRSCMessageSerial(const int nSendID,const int nSerial);


    void ReSetListCountNum();


    //////////////////////////////////////////////////////////////////////////
    // 通信串口操作
    //////////////////////////////////////////////////////////////////////////
    // 数据输出
    bool PortCommSendOutData(char* pChar,int nLen);




    void VHFLayerChangeClientToHead();

private:
    CE_VHFNodeManage();
    ~CE_VHFNodeManage();

signals:
    void comSendData(char* pChar,int nLen);

private slots:
    void OnCommRecData(const QByteArray &data);

    void dealVHFLayerTimer();

public:
    static CE_VHFNodeManage*    m_instance;
    static QMutex               m_mutex;

    QMutex                      m_listMutex;

    CSC_01LayerHead*            m_pLayerVHFHead;	// VHF Layer head
    CSC_01LayerClient*          m_pLayerVHFClient;	// VHF Layer Client

    QTimer*                     m_VHFLayerTimer;

    unsigned char*				m_pExDataSend;		// VHF Layer Send Data buffer
    int                         m_nExDataSLen;		// VHF Layer Send Data Buffer Length
    // Send Messge Buffer
    NET_MSG_HEADER              m_sSendHead;			// Send header STRUCT
    unsigned char*              m_pBufSend;				// 发送数据缓存
    int                         m_nBufSendLen;			// 发送数据缓存长度

    unsigned char*              m_MRTPosData;
    int                         m_MRTPosDataLen;

    int                         m_nVHFIDMe;				// VHF ID Me
};

#endif // CE_VHFNODEMANAGE_H

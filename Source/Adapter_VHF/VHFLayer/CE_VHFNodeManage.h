#ifndef CE_VHFNODEMANAGE_H
#define CE_VHFNODEMANAGE_H

#include <QObject>
#include <QSharedPointer>
#include <QList>
#include <QMutex>
#include "common.h"

#define RADIORTCCMDLENGTH 2048

class CSC_01LayerHead;
class CSC_01LayerClient;
class CSCRSC_ObjVHFMsg;
class CSC_01Layer;
class QTimer;

typedef QSharedPointer<CSCRSC_ObjVHFMsg> pVHFMsg;

class CE_VHFNodeManage : public QObject
{
    Q_OBJECT
public:
    static CE_VHFNodeManage* getInstance();
    void init();

    //////////////////////////////////////////////////////////////////////////
    // 信息交互操作
    //////////////////////////////////////////////////////////////////////////
    // ==>> 向下发送
    // 发送报文
    void ACCtoRSCMessageData(const int nSendID, const int nRecvID, char* pChar,const int nLen, bool bEncrypt,int nDegree, const int nSerial);

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
    // VHF Layer Control
    CSC_01Layer* GetLayerVHF();
    void VHFLayerChangeClientToHead();
    // No.4 Condition
    // Get the Layer Message And Change the State
    ////////////////////////////////////////////////////
    // VHF Layer Get the Waiting Send Message
    // Return Value:
    //		  -2:Not Find the Receive ID
    //		  -1:Pointer Not Exist
    //		   0:Have Been Out the Length Limit
    //         1:Still Have Space to Add the Data
    //         2:No Data Need be Sended
    int VHFLayerSendDataFromListWait(int maxlength);
    // Append Message Data to VHF Send Data
    // Return Value:
    //		  -2:Not Find the Receive ID
    //		  -1:Pointer Not Exist
    //		   0:Have Been Out the Length Limit
    //         1:Still Have Space to Add the Data
    int XSetVHFDataAppend(CSCRSC_ObjVHFMsg& stumsg,int maxlength);


    int XSetVHFMRTPosDataAppend(unsigned char* pPosData,int PosDataLen,int sendId,int recvID,int serial,int maxlength);

    bool DeleteACCtoRSCMessageData(const int nSendID, const int nSerialBegin, const int nSerialEnd);
    void ReSetListCountNum();


    //////////////////////////////////////////////////////////////////////////
    // 通信串口操作
    //////////////////////////////////////////////////////////////////////////
    // 数据输出
    bool PortCommSendOutData(char* pChar,int nLen);


    void OnCommRecData(const QByteArray &data);


private:
    CE_VHFNodeManage();
    ~CE_VHFNodeManage();

signals:
    void comSendData(char* pChar,int nLen);

private slots:


    void dealVHFLayerTimer();

public:
    CSC_01LayerHead*            m_pLayerVHFHead;        // VHF Layer head
    CSC_01LayerClient*          m_pLayerVHFClient;      // VHF Layer Client

    unsigned char*				m_pExDataSend;          // VHF Layer Send Data buffer
    int                         m_nExDataSLen;          // VHF Layer Send Data Buffer Length
    // Send Messge Buffer
    NET_MSG_HEADER              m_sSendHead;			// Send header STRUCT
    unsigned char*              m_pBufSend;				// 发送数据缓存
    int                         m_nBufSendLen;			// 发送数据缓存长度

    unsigned char*              m_MRTPosData;
    int                         m_MRTPosDataLen;

    int                         m_nVHFIDMe;				// VHF ID Me

    QList<pVHFMsg>              m_lVHFMsgList;

private:
    static CE_VHFNodeManage*    m_instance;
    static QMutex               m_mutex;

    QTimer*                     m_VHFLayerTimer;
    QMutex                      m_listMutex;
};

#endif // CE_VHFNODEMANAGE_H

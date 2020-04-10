#ifndef RADIOLINKMANAGE_H
#define RADIOLINKMANAGE_H

#include <QObject>
#include <QMutex>
#include "LinkCommon.h"

class RadioLink;
class RadioLinkMaster;
class RadioLinkClient;
class QTimer;

typedef QSharedPointer<ObjMsg> pVHFMsg;

enum	RSCMessage_Degree	// Message Degree
{
    MSGDEG_URGENT = 1,			// Urgent Message
    MSGDEG_GENERAL,				// General Message
    MSGDEG_CYCLE,				// Cycle Message
    MSGDEG_RELAX,				// Relax Message
    MSGDEG_ENDCYCLE,			// 末端循环
};

class RadioLinkManage : public QObject
{
    Q_OBJECT
public:
    static RadioLinkManage* getInstance();
    void init();

    inline RadioLinkClient* getRadioLinkClient() { return m_radioLinkClient; }

    //////////////////////////////////////////////////////////////////////////
    // 信息交互操作
    //////////////////////////////////////////////////////////////////////////
    // ==>> 向下发送
    // 发送报文
    void ACCtoRSCMessageData(const int nSendID, const int nRecvID, char* pChar,const int nLen, bool bEncrypt,int nDegree, const int nSerial);
    //移动台位置报文
    void ACCtoRSCPosData(const char* pchar, const int len);
    //删除报文
    bool DeleteACCtoRSCMessageData(const int nSendID, const int nSerialBegin, const int nSerialEnd);
    // 向上反馈 ==>>
    // 上报当前接收到的报文
    void PackToSendRMTtoRSCMessageData(const int nSendID,const int nRecvID,char* pChar,const int nLen,bool bEncrypt);
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
    bool PortCommSendOutData(char* pChar, int nLen);
    void OnCommRecData(const QByteArray &data);

    // No.4 Condition
    // Get the Layer Message And Change the State
    ////////////////////////////////////////////////////
    // Get the Waiting Send Message
    // Return Value:
    //		  -2:Not Find the Receive ID
    //		  -1:Pointer Not Exist
    //		   0:Have Been Out the Length Limit
    //         1:Still Have Space to Add the Data
    //         2:No Data Need be Sended
    int sendDataFromListWait(int maxlength);

    void changeClientToMaster();

private:
    RadioLinkManage();
    ~RadioLinkManage();

    // VHF Layer Control
    RadioLink* GetLayer();

    // Append Message Data to Send Data
    // Return Value:
    //		  -2:Not Find the Receive ID
    //		  -1:Pointer Not Exist
    //		   0:Have Been Out the Length Limit
    //         1:Still Have Space to Add the Data
    int setDataAppend(ObjMsg& stumsg,int maxlength);
    int setMRTPosDataAppend(char* pPosData, int PosDataLen, int sendId, int recvID, int serial, int maxlength);

private slots:
    void dealTimer();
    void msgListProcess();

private:
    static RadioLinkManage*         m_instance;
    static QMutex                   m_mutex;

    RadioLinkMaster*                m_radioLinkMaster;
    RadioLinkClient*                m_radioLinkClient;

    QTimer*                         m_timer;
    QTimer*                         m_listTimer;
    QMutex                          m_listMutex;

    char*                           m_pExDataSend;          // VHF Layer Send Data buffer
    int                             m_nExDataSLen;          // VHF Layer Send Data Buffer Length
    // Send Messge Buffer
    NET_MSG_HEADER                  m_sSendHead;			// Send header STRUCT
    char*                           m_pBufSend;				// 发送数据缓存
    int                             m_nBufSendLen;			// 发送数据缓存长度

    char*                           m_MRTPosData;
    int                             m_MRTPosDataLen;

    int                             m_nIDMe;				//ID Me

    QList<pVHFMsg>                  m_lMsgList;
};

#endif // RADIOLINKMANAGE_H

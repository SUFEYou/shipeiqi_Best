#ifndef RADIOLINK_H
#define RADIOLINK_H

#include <QByteArray>
#include <QList>
#include "LinkCommon.h"

class RadioLink
{
public:
    RadioLink();
    virtual ~RadioLink();

    enum{					// Layer State Information
        LAYSTA_ONLINE = 1,		// Stage is on Line
        LAYSTA_OFFLINE,			// Stage is Off Line
        LAYSTA_SUCCESS,			// Receive the Stage's Data Success
        LAYSTA_LOST,			// Lost the Stage's Data
    };

    enum{						// Layer Moment Information
        MOMENT_LISTEN = 1,			// Listen Moment, Not in the Form
        MOMENT_BEGIN,				// Begin Moment
        MOMENT_CIRCLE,				// Circle Moment
        MOMENT_APPLY,				// Apply Moment
        MOMENT_DRIFT,				// Drift Moment
    };

    enum{						// Message Type Of the Layer
        LAYMSG_CONTROL = 1,			// Message to Control the Layer's Information
        LAYMSG_STATE,				// Layer State Control Message
        LAYMSG_MSGONCE,				// Layer's transmit Information
        LAYMSG_MSGCAST,				// Broadcast Information
        LAYMSG_MSGCALL,				// Message's Recall
    };

    enum						// Apply State
    {
        LAYAPP_ONLINE = 1,			// Apply to Join the Chain
        LAYAPP_OFFLINE,				// Apply to Leave the Chain
        LAYAPP_ONCE,				// Once Send Message
        LAYAPP_STILL,				// Call that online
    };

public:
// Operate
    //////////////////////////////////////////////////////////////////////////
    // Set the class is or not Available
    void SetAvailable(bool available);
    bool GetAvailable();

    void recvData(const char* pchar,const int nlength);

    // Encode the Data Layer Sentence
    int DataLayerSendMemoryGatherJudge(bool bencrypt,const int nlength,int maxlength);
    void DataLayerSendMemoryGather(char*pchar,const int nlength);

    //广播报编码
    void castMessageEncode(char* psource,int nlength,bool bencrypt,int nsource,int naim,
            char* pencode,int& enlen);
    //单发报编码
    void onceMessageEncode(char* psource,int nlength,bool bencrypt,int nsource,int naim,
            char* pencode,int& enlen,int& serial);

    inline ModelChain& getModelChain() { return m_nChain; }
    inline void setModelChain(ModelChain& x) { m_nChain = x; }
    inline int getCodeMe() { return m_nCodeMe; }
    inline void setCodeMe(int x) { m_nCodeMe = x; }
    inline int getDataMaxLen() { return m_nDataMaxLen; }
    inline void setDataMaxLen(int x) { m_nDataMaxLen = x; }
    inline int getTimeFactor() { return m_nTimeFactor; }
    inline void setTimeFactor(int x) { m_nTimeFactor = x; }
    inline int getCircleDrift() { return m_nTCircleDrift; }
    inline void setCircleDrift(int x) { m_nTCircleDrift = x; }
    inline bool getMonitorAll() { return m_bMonitorAll; }
    inline void setMonitorAll(bool x) { m_bMonitorAll = x; }
    inline int getMeState() { return m_nMeState; }
    inline void setMeState(int x) { m_nMeState = x; }
    inline int getChainIDGo() { return m_nChainIDGo; }
    inline void setChainIDGo(int x) { m_nChainIDGo = x; }
    inline bool getSendOK() { return m_bSendOK; }
    inline void setSendOK(bool x) { m_bSendOK = x; }

    //////////////////////////////////////////////////////////////////////////
    // Link Layer Circle
    // Main Control
    virtual void LinkLayerMainCircle() = 0;
    //////////////////////////////////////////////////////////////////////////
    // Layer Into Moment
    virtual void LinkLayerCircleMomentToBegin() = 0;
    virtual void LinkLayerCircleMomentToCircle() = 0;
    virtual void LinkLayerCircleMomentToApply() = 0;
    virtual void LinkLayerCircleMomentToDrift() = 0;

    virtual void timerProcess() = 0;

protected:
    virtual void recvDataAnalyze(ObjMsg &recvMsg) = 0;

    // Pack Data into Layer Message
    void DataLayerMessageEncode(ObjMsg& msg,char* pencode,int& enlen);

    // Do With the Step Touch
    bool LinkLayerComStepTouch(int ncurid);

    //////////////////////////////////////////////////////////////////////////
    // Data Layer Sentence
    // Pack Data Save in :: m_pExData+m_pExDataLen
    //////////////////////////////////////////////////////////////////////////
    // LAYMSG_CONTROL :: Message to Control the Layer's Information
    bool ActSenLAYMSG_CONTROLPack(ModelChain& chain);
    bool ActSenLAYMSG_CONTROLUnpack(char* pchar,const int nlength);
    // LAYMSG_STATE :: Layer State Control Message
    bool ActSenLAYMSG_STATEPack(int nstate,QString strdesc);
    bool ActSenLAYMSG_STATEUnpack(char* pchar,const int nlength);
    // LAYMSG_MSGCAST :: Broadcast Information
    bool ActSenLAYMSG_MSGCASTPack(char* pdata,int nlength);
    bool ActSenLAYMSG_MSGCASTUnpack(char* pchar,const int nlength);
    // LAYMSG_MSGONCE :: Layer's transmit Information
    bool ActSenLAYMSG_MSGONCEPack(char* pdata,int nlength,int& nserial);
    bool ActSenLAYMSG_MSGONCEUnpack(char* pchar,const int nlength);
    // LAYMSG_MSGCALL :: Message's Recall
    bool ActSenLAYMSG_MSGCALLPack(int nmaxlen);
    bool ActSenLAYMSG_MSGCALLUnpack(char* pchar,const int nlength);

    // Apply to Enjoy or Leave the Chain
    bool DataLayerMessageStateApply(int nstate,QString strdesc);

    //////////////////////////////////////////////////////////////////////////
    // Data Interface
    //////////////////////////////////////////////////////////////////////////
    // Send Out Data to Terminal
    bool ComSendOutData(char* pchar,int nlength);

private:
    void recvDataSubpackage();
    void recvDataParse();

    // Link Layer Get Apply Raid Time
    int LinkLayerGetApplyTime(bool availabe,int maxvalue);

    // Encrypt the Message
    // 创建密钥，生成加密报文
    void ActEncrypt_KeyCreate(char* pchar,const int nlength);
    // 解密数据
    bool ActEncrypt_KeyOpen(char* pchar,const int nlength);
    // 生成加密码
    void ActEncrypt_CodeCreate(char A,char B,char C);
    // 去处数据中指定的字符fix
    void ActEncrypt_CharClear(char* pchar,const int nlength,char& nEmp,const char fix);
    // 恢复数据中的指定字符fix
    void ActEncrypt_CharAdd(char* pchar,const int nlength,const char nEmp);

protected:
    //////////////////////////////////////////////////////////////////////////
    // Base Layer Information
    int                                 m_nCodeMe;		// My Stage Code
    int                                 m_nCodeVer;		// Version Code
    bool                                m_bAvailable;	// Available
    int                                 m_nDataMaxLen;	// Data Layer Send Message's Maximum Length

    // Link Layer Control  Parameter
    ModelChain                          m_nChain;	// Main Chain
    int                                 m_nMeState;			// Me's State,can or Not Send Message
    int                                 m_nChainIDNow;		// Now Chain ID
    int                                 m_nChainIDGo;		// // Want to Enjoin the Chain
    int                                 m_nMoment;			// Running Moment,Listen/Begin/Circle/Apply/Drift
    int                                 m_nTApply;			// Save the Random Apply Moment's time
    int                                 m_nTCircleDrift;	// Set After the Stage Receive the Message, Drift to Send message Time
    int                                 m_nTCircleDriftCount;	// Circle Drift Count
    bool                                m_bTCircleSend;		// Is or Not Can Send the Message

    bool                                m_bChainCircleFlag;	// In or Out the Chain Communicate

    int                                 m_nTOutCount;		// Out Time Count
    int                                 m_nTimeFactor;		// Time Step Factor

    int                                 m_nSeatPrv;			// Previous	Send Message's Seat Number
    int                                 m_nSeatNow;			// Now Send Message's Seat Number

    // Link Layer Sentence Parameter
    bool                                m_bSendOK;			// Is or Not Send Message Success
    bool                                m_bSendApplyCan;	// Is or Not can be Send in the Apply moment
    bool                                m_bMonitorAll;		// Is or Not can Monitor Layer Message

    // Message Translate Parameter
    int                                 m_nSerial;			// Message's Serial

    // Send Message Later
    int                                 m_nSendLater;

    // LAYMSG_MSGCALL
    QList<pObjRecall>                   m_nListRecall;

    //////////////////////////////////////////////////////////////////////////
    // Receive the Message Data Information
    ObjMsg                              m_nRecvMsg;		// Receive Message Data

    // LAYMSG_CONTROL :: Layer Control Message
    ModelChain                          m_nRecvChain;	// Save the Receive Chain

    // LAYMSG_MSG  :: Message Data
    char*                               m_pMsgRecvData;          // Save the Layer Message's User Data
    int                                 m_pMsgRecvLen;			// Receive Data's Length
    int                                 m_pMsgRecvSn;			// Receive Message's Serial

    // LAYMSG_STATE :: State
    int                                 m_nRecvState;			// Apply State
    QString                             m_strStateDesc;			// Apply Describe

    // LAYMSG_MSGCALL :: Recall
    QList<pObjRecall>                    m_nRecvCallList;

    //////////////////////////////////////////////////////////////////////////
    // Data Layer Base Parameter, for Exchange
    // Save Data to Send to Layer
    char*                               m_pDSendData;		// Save the Data to Send to Layer
    int                                 m_pDSendLen;			// Send Data's Length

    // Save Apply Data to Send to Layer
    char*                               m_pDApplyData;		// Apply Data
    int                                 m_pDApplyLen;

    // Save the Encrypt Code
    char                                EncryptKey[100];		// Encrypt Code

    // Template save Exchange Pack Data
    char*                               m_pExData;			// Exchange Data Space
    int                                 m_pExDataLen;		// Exchange Data Space Length


    //////////////////////////////////////////////////////////////////////////
    // Monitor the Member State
    int                                 m_nMonCode;			// Monitoring State Code
    QTime                               m_nMonRecvTime;     // Receive the Message Time

private:
    QByteArray                          m_recvDataArray;
    QList<QByteArray>                   m_recvDataList;

};

#endif // RADIOLINK_H

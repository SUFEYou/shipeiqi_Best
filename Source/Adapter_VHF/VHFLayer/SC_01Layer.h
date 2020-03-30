#ifndef SC_01LAYER_H
#define SC_01LAYER_H

#include <QSharedPointer>
#include <QList>
#include <QTime>
#include "VHFLayer/TerminalBase.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// VHF Layer Control Class
//////////////////////////////////////////////////////////////////////////

#define PACK_LENGTHLIMIT  1020

class CSCObjStage	// Stage Information
{
public:
    //////////////////////////////////////////////////////////////////////////
    // Operate
    CSCObjStage();
    void operator =(CSCObjStage& obj);
    // Clear
    void Clear();

    //////////////////////////////////////////////////////////////////////////
    // Parameter
    int		id;			// Stage ID
    int		state;		// Stage State, Online or Offline
    bool	signal;		// signal is or Not can be Received
    int		degree;		// Degree,  1 = Main/2 = Assistant
    int		sequence;	// Sequence
    int		errcount;	// Error Count,Master Stage Use it

protected:
private:
};

typedef QSharedPointer<CSCObjStage> pCSCObjStage;

class CSCModelChain
{
public:
    //////////////////////////////////////////////////////////////////////////
    // Operate
    CSCModelChain();
    void operator =(CSCModelChain& chain);
    // Clear
    void Clear();
    // Get Member is or not in the List
    bool IsInList(int nid,int& nstate);

    //////////////////////////////////////////////////////////////////////////
    // Parameter
    int                 nChainId;			// Chain ID
    int                 nLimitOut;			// Out Time Limit
    int                 nLimitApply;		// Apply Moment's time limit
    int                 nLimitDrift;		// Drift Moment's time limit

    int                 nMemNum;			// Member's Number
    QList<pCSCObjStage> nListMember;        // Member List on the Chain

protected:
private:
};

class CSCRSC_ObjVHFMsg
{
public:
    //////////////////////////////////////////////////////////////////////////
    // Operate
    CSCRSC_ObjVHFMsg();
    void operator =(CSCRSC_ObjVHFMsg& msg);
    // Clear
    void Clear();
    // Get Message Type
    int GetDataType();

    //////////////////////////////////////////////////////////////////////////
    // Parameter
    int		nReceive;		// Receive Id
    int		nSource;		// Source id
    bool	bEncrypt;		// Encrypt
    int		nVersion;		// Version
    int		nDataLen;		// Message Length
    char	pData[255];		// Message Data
    int		nDegree;		// Degree
    int		nSerial;		// Message Serial
    int		nTimeCount;		// Time Count
    int		nSendtimes;		// Send Times
protected:
private:
};

class CSCObjRecall
{
public:
    //////////////////////////////////////////////////////////////////////////
    // Operate
    CSCObjRecall();
    void operator =(CSCObjRecall& msg);

    //////////////////////////////////////////////////////////////////////////
    // Parameter
    int nSource;		// Message Source
    int	nSerial;		// Message's Serial，VHF链路生成的报文序列号
    int	nState;			// 报文状态
protected:
private:
};

typedef QSharedPointer<CSCObjRecall> pCSCObjRecall;

class CSC_01Layer : public CTerminalBase
{
public:
    CSC_01Layer();
    ~CSC_01Layer();

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
    //////////////////////////////////////////////////////////////////////////
    // Get the Link Layer Information
    //////////////////////////////////////////////////////////////////////////
    // Judge One Can or Not Receive Data
    bool LinkLayerGetStageCanRecvData(int stage);
    // Get My is or Not can send Message in the chain
    bool LinkLayerGetStageCanSendData(int stage);
    // Link Layer Get Apply Raid Time
    int LinkLayerGetApplyTime(bool availabe,int maxvalue);
    // Set Stage On or Off Line
    bool LinkLayerSetStageLineState(int nstage,int nstate);

    // Do With the Step Touch
    bool LinkLayerComStepTouch(int ncurid);

    //////////////////////////////////////////////////////////////////////////
    // Data Interface
    //////////////////////////////////////////////////////////////////////////
    // Send Out Data to Terminal
    bool ComSendOutData(char* pchar,int nlength);

public:

    // Pack Data into Layer Message
    void DataLayerMessageEncode(CSCRSC_ObjVHFMsg& msg,char* pencode,int& enlen);
    void DataLayerMessageCastEncode(char* psource,int nlength,bool bencrypt,int nsource,int naim,
            char* pencode,int& enlen);
    void DataLayerMessageOnceEncode(char* psource,int nlength,bool bencrypt,int nsource,int naim,
            char* pencode,int& enlen,int& serial);
    // Parse the Layer Message Data
    bool DataLayerMessageParse(char* pchar,const int nlength,CSCRSC_ObjVHFMsg& msg);

    // Encode the Data Layer Sentence
    int DataLayerSendMemoryGatherJudge(bool bencrypt,const int nlength,int maxlength);
    void DataLayerSendMemoryGather(char*pchar,const int nlength);

    // Encode the Apply Data Layer Sentence
    int DataLayerSendMemoryApplyGather(char*pchar,const int nlength);

    // Apply to Enjoy or Leave the Chain
    bool DataLayerMessageStateApply(int nstate,QString strdesc);

    // Create the Serial
    int	DataLayerSerialCreat();

    //////////////////////////////////////////////////////////////////////////
    // Data Layer Sentence
    // Pack Data Save in :: m_pExData+m_pExDataLen
    //////////////////////////////////////////////////////////////////////////
    // LAYMSG_CONTROL :: Message to Control the Layer's Information
    bool ActSenLAYMSG_CONTROLPack(CSCModelChain& chain);
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

// Parameter
public:

    //////////////////////////////////////////////////////////////////////////
    // Base Layer Information
    int		m_nCodeMe;		// My Stage Code
    int		m_nCodeVer;		// Version Code
    bool	m_bAvailable;	// Available
    int		m_nDataMaxLen;	// Data Layer Send Message's Maximum Length

    // Link Layer Control  Parameter
    CSCModelChain	m_nChain;	// Main Chain
    int		m_nMeState;			// Me's State,can or Not Send Message
    int		m_nChainIDNow;		// Now Chain ID
    int		m_nChainIDGo;		// // Want to Enjoin the Chain
    int		m_nMoment;			// Running Moment,Listen/Begin/Circle/Apply/Drift
    int		m_nTApply;			// Save the Random Apply Moment's time
    int		m_nTCircleDrift;	// Set After the Stage Receive the Message, Drift to Send message Time
    int		m_nTCircleDriftCount;	// Circle Drift Count
    bool	m_bTCircleSend;		// Is or Not Can Send the Message

    bool	m_bChainCircleFlag;	// In or Out the Chain Communicate

    int		m_nTOutCount;		// Out Time Count
    int		m_nTimeFactor;		// Time Step Factor

    int		m_nSeatPrv;			// Previous	Send Message's Seat Number
    int		m_nSeatNow;			// Now Send Message's Seat Number

    // Link Layer Sentence Parameter
    bool 	m_bSendOK;			// Is or Not Send Message Success
    bool	m_bSendApplyCan;	// Is or Not can be Send in the Apply moment
    bool	m_bMonitorAll;		// Is or Not can Monitor Layer Message

    // Message Translate Parameter
    int		m_nSerial;			// Message's Serial

    // Send Message Later
    int		m_nSendLater;

    // LAYMSG_MSGCALL
    QList<pCSCObjRecall>        m_nListRecall;
    //CList<CSCObjRecall,CSCObjRecall&>		m_nListRecall;

    //////////////////////////////////////////////////////////////////////////
    // Receive the Message Data Information
    CSCRSC_ObjVHFMsg	m_nRecvMsg;		// Receive Message Data

    // LAYMSG_CONTROL :: Layer Control Message
    CSCModelChain	m_nRecvChain;	// Save the Receive Chain

    // LAYMSG_MSG  :: Message Data
    char*	   m_pMsgRecvData;          // Save the Layer Message's User Data
    int		   m_pMsgRecvLen;			// Receive Data's Length
    int		   m_pMsgRecvSn;			// Receive Message's Serial

    // LAYMSG_STATE :: State
    int			m_nRecvState;			// Apply State
    QString		m_strStateDesc;			// Apply Describe

    // LAYMSG_MSGCALL :: Recall
    QList<pCSCObjRecall>	m_nRecvCallList;
    //CList<CSCObjRecall,CSCObjRecall&>	m_nRecvCallList;

    //////////////////////////////////////////////////////////////////////////
    // Data Layer Base Parameter, for Exchange
    // Save Data to Send to Layer
    char*	   m_pDSendData;		// Save the Data to Send to Layer
    int		   m_pDSendLen;			// Send Data's Length

    // Save Apply Data to Send to Layer
    char*		m_pDApplyData;		// Apply Data
    int			m_pDApplyLen;

    // Save the Encrypt Code
    char	   EncryptKey[100];		// Encrypt Code

    // Template save Exchange Pack Data
    char*	   m_pExData;			// Exchange Data Space
    int		   m_pExDataLen;		// Exchange Data Space Length


    //////////////////////////////////////////////////////////////////////////
    // Monitor the Member State
    int             m_nMonCode;			// Monitoring State Code
    QTime           m_nMonRecvTime;     // Receive the Message Time
};

#endif // SC_01LAYER_H

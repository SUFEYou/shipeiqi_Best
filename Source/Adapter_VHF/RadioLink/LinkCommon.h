#ifndef LINKCOMMON_H
#define LINKCOMMON_H

#include <QSharedPointer>
#include <QList>
#include <QTime>

#define PACK_LENGTHLIMIT  1020

class ObjStage;
class ObjRecall;

typedef QSharedPointer<ObjStage> pObjStage;
typedef QSharedPointer<ObjRecall> pObjRecall;

class ObjStage	// Stage Information
{
public:
    //////////////////////////////////////////////////////////////////////////
    // Operate
    ObjStage();
    void operator =(ObjStage& obj);
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

class ModelChain
{
public:
    //////////////////////////////////////////////////////////////////////////
    // Operate
    ModelChain();
    void operator =(ModelChain& chain);
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
    QList<pObjStage> nListMember;        // Member List on the Chain

protected:
private:
};

class ObjMsg
{
public:
    //////////////////////////////////////////////////////////////////////////
    // Operate
    ObjMsg();
    void operator =(ObjMsg& msg);
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

class ObjRecall
{
public:
    //////////////////////////////////////////////////////////////////////////
    // Operate
    ObjRecall();
    void operator =(ObjRecall& msg);

    //////////////////////////////////////////////////////////////////////////
    // Parameter
    int nSource;		// Message Source
    int	nSerial;		// Message's Serial，VHF链路生成的报文序列号
    int	nState;			// 报文状态
protected:
private:
};



#endif // LINKCOMMON_H

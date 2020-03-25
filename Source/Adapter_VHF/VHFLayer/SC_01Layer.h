#ifndef SC_01LAYER_H
#define SC_01LAYER_H

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
    int		nChainId;			// Chain ID
    int		nLimitOut;			// Out Time Limit
    int		nLimitApply;		// Apply Moment's time limit
    int		nLimitDrift;		// Drift Moment's time limit

    int		nMemNum;			// Member's Number
    //CList<CSCObjStage,CSCObjStage&>		nListMember;	// Member List on the Chain

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

#endif // SC_01LAYER_H

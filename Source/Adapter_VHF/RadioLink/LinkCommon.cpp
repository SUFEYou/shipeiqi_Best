#include "LinkCommon.h"

ObjStage::ObjStage()
{
    id		= 0;		// Stage ID
    state	= 0;		// Stage State, Online or Offline
    signal	= false;	// signal is or Not can be Received
    degree	= 0;		// Degree,  1 = Main/2 = Assistant
    sequence	= 0;	// Sequence
    errcount	= 0;	// Error Count,Master Stage Use it

}

void ObjStage::operator =(ObjStage& obj)
{
    id		= obj.id;			// Stage ID
    state	= obj.state;		// Stage State, Online or Offline
    signal	= obj.signal;		// signal is or Not can be Received
    degree	= obj.degree;		// Degree,  1 = Main/2 = Assistant
    sequence	= obj.sequence;	// Sequence
    errcount	= obj.errcount;	// Error Count,Master Stage Use it
}

// Clear
void ObjStage::Clear()
{
    id		= 0;		// Stage ID
    state	= 0;		// Stage State, Online or Offline
    signal	= false;	// signal is or Not can be Received
    degree	= 0;		// Degree,  1 = Main/2 = Assistant
    sequence	= 0;	// Sequence
    errcount	= 0;	// Error Count,Master Stage Use it
}

//////////////////////////////////////////////////////////////////////////
// Class CSCModelChain
// Operate
ModelChain::ModelChain()
{
    nChainId	= 0;			// Chain ID
    nLimitOut	= 3;			// Out Time Limit
    nLimitApply	= 5;            // Apply Moment's time limit
    nLimitDrift	= 2;            // Drift Moment's time limit

    nMemNum		= 0;			// Member's Number
    nListMember.clear();        // Member List on the Chain
}

void ModelChain::operator =(ModelChain& chain)
{
    nChainId	= chain.nChainId;			// Chain ID
    nLimitOut	= chain.nLimitOut;			// Out Time Limit
    nLimitApply	= chain.nLimitApply;		// Apply Moment's time limit
    nLimitDrift	= chain.nLimitDrift;		// Drift Moment's time limit

    nMemNum		= chain.nMemNum;			// Member's Number
    nListMember.clear();                    // Member List on the Chain
    for(int i = 0; i < chain.nListMember.length(); ++i)
    {
        pObjStage obj = chain.nListMember[i];
        nListMember.push_back(obj);
    }
}

// Clear
void ModelChain::Clear()
{
    nChainId	= 0;			// Chain ID
    nLimitOut	= 0;			// Out Time Limit
    nLimitApply	= 0;            // Apply Moment's time limit
    nLimitDrift	= 0;            // Drift Moment's time limit

    nMemNum		= 0;			// Member's Number
    nListMember.clear();        // Member List on the Chain
}

// Get Member is or not in the List
bool ModelChain::IsInList(int nid,int& nstate)
{
    nstate = -1;

    for(int i = 0; i < nListMember.length(); ++i)
    {
        pObjStage obj = nListMember[i];
        if (obj->id == nid)
        {
            nstate = obj->state;
            return true;
        }
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////
// Class CSCRSC_ObjVHFMsg
// Operate
ObjMsg::ObjMsg()
{
    nReceive	= 0;		// Receive Id
    nSource		= 0;		// Source id
    bEncrypt	= false;	// Encrypt
    nVersion	= 0;		// Version
    nDataLen	= 0;		// Message Length
    memset(pData, 0, 255);	// Message Data
    nDegree		= 0;
    nSerial		= 0;
    nTimeCount	= 0;
    nSendtimes	= 0;
}

void ObjMsg::operator =(ObjMsg& msg)
{
    nReceive	= msg.nReceive;		// Receive Id
    nSource		= msg.nSource;		// Source id
    bEncrypt	= msg.bEncrypt;		// Encrypt
    nVersion	= msg.nVersion;		// Version
    nDataLen	= msg.nDataLen;		// Message Length
    memcpy(pData, msg.pData, 255);	// Message Data
    nDegree		= msg.nDegree;
    nSerial		= msg.nSerial;
    nTimeCount	= msg.nTimeCount;
    nSendtimes	= msg.nSendtimes;
}

// Clear
void ObjMsg::Clear()
{
    nReceive	= 0;		// Receive Id
    nSource		= 0;		// Source id
    bEncrypt	= false;	// Encrypt
    nVersion	= 0;		// Version
    nDataLen	= 0;		// Message Length
    memset(pData, 0, 255);	// Message Data
    nDegree		= 0;
    nSerial		= 0;
    nTimeCount	= 0;
    nSendtimes	= 0;
}

// Get Message Type
int ObjMsg::GetDataType()
{
    return pData[0];
}

//////////////////////////////////////////////////////////////////////////
// Class CSCObjRecall
//////////////////////////////////////////////////////////////////////////
ObjRecall::ObjRecall()
{
    nSource = 0;
    nSerial	= 0;
    nState  = 0;
}

void ObjRecall::operator =(ObjRecall& msg)
{
    nSource = msg.nSource;
    nSerial = msg.nSerial;
    nState	= msg.nState;
}


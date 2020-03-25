#include "VHFLayer/SC_01Layer.h"
#include <memory.h>

//////////////////////////////////////////////////////////////////////////
// Class CSCObjStage
// Operate
CSCObjStage::CSCObjStage()
{
    id		= 0;		// Stage ID
    state	= 0;		// Stage State, Online or Offline
    signal	= false;	// signal is or Not can be Received
    degree	= 0;		// Degree,  1 = Main/2 = Assistant
    sequence	= 0;	// Sequence
    errcount	= 0;	// Error Count,Master Stage Use it

}

void CSCObjStage::operator =(CSCObjStage& obj)
{
    id		= obj.id;			// Stage ID
    state	= obj.state;		// Stage State, Online or Offline
    signal	= obj.signal;		// signal is or Not can be Received
    degree	= obj.degree;		// Degree,  1 = Main/2 = Assistant
    sequence	= obj.sequence;	// Sequence
    errcount	= obj.errcount;	// Error Count,Master Stage Use it
}

// Clear
void CSCObjStage::Clear()
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
CSCModelChain::CSCModelChain()
{
    nChainId	= 0;			// Chain ID
    nLimitOut	= 3;			// Out Time Limit
    nLimitApply	= 5;		// Apply Moment's time limit
    nLimitDrift	= 2;		// Drift Moment's time limit

    nMemNum		= 0;			// Member's Number
    //nListMember.RemoveAll();	// Member List on the Chain
}

void CSCModelChain::operator =(CSCModelChain& chain)
{
    nChainId	= chain.nChainId;			// Chain ID
    nLimitOut	= chain.nLimitOut;			// Out Time Limit
    nLimitApply	= chain.nLimitApply;		// Apply Moment's time limit
    nLimitDrift	= chain.nLimitDrift;		// Drift Moment's time limit

    nMemNum		= chain.nMemNum;			// Member's Number
//	nListMember.RemoveAll();	// Member List on the Chain
//	POSITION pos = chain.nListMember.GetHeadPosition();
//	while (pos)
//	{
//		CSCObjStage obj = chain.nListMember.GetNext(pos);
//		nListMember.AddTail(obj);
//	}
}

// Clear
void CSCModelChain::Clear()
{
    nChainId	= 0;			// Chain ID
    nLimitOut	= 0;			// Out Time Limit
    nLimitApply	= 0;		// Apply Moment's time limit
    nLimitDrift	= 0;		// Drift Moment's time limit

    nMemNum		= 0;			// Member's Number
    //nListMember.RemoveAll();	// Member List on the Chain
}

// Get Member is or not in the List
bool CSCModelChain::IsInList(int nid,int& nstate)
{
    nstate = -1;

//	POSITION pos = nListMember.GetHeadPosition();
//	while (pos)
//	{
//		CSCObjStage& obj = nListMember.GetNext(pos);
//		if (obj.id == nid)
//		{
//			nstate = obj.state;
//			return true;
//		}
//	}

    return false;
}

//////////////////////////////////////////////////////////////////////////
// Class CSCRSC_ObjVHFMsg
// Operate
CSCRSC_ObjVHFMsg::CSCRSC_ObjVHFMsg()
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

void CSCRSC_ObjVHFMsg::operator =(CSCRSC_ObjVHFMsg& msg)
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
void CSCRSC_ObjVHFMsg::Clear()
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
int CSCRSC_ObjVHFMsg::GetDataType()
{
    return pData[0];
}

#ifndef CSC_01LAYERCLIENT_H
#define CSC_01LAYERCLIENT_H

#include "VHFLayer/SC_01Layer.h"

class CSC_01LayerClient: public CSC_01Layer
{
public:
    CSC_01LayerClient();
    virtual ~CSC_01LayerClient();

public:
    //////////////////////////////////////////////////////////////////////////
    //	User Layer Control
    //////////////////////////////////////////////////////////////////////////
    // Interface to User,Set the Stage & Layer Information
    // Input the Layer Information
    void UseDataInput(const char* pchar,const int nlength);

private:
    //////////////////////////////////////////////////////////////////////////
    // Analyze Data Layer Char
    //////////////////////////////////////////////////////////////////////////
    // Analyze the Sentence
    void ActAnalyzeVHFLayer(unsigned char cRec);
    void ActAnalyzeError();

public:
    // Translate the Data Layer Sentence
    bool DataLayerMessageAnalyze(char* pchar,const int nlength);

    //////////////////////////////////////////////////////////////////////////
    // Link Layer Control
    //////////////////////////////////////////////////////////////////////////
    // Timer Control Interface
    void XTimeMaintenanceCircle();

    //////////////////////////////////////////////////////////////////////////
    // Link Layer Circle
    // Main Control
    void LinkLayerMainCircle();
    // Send Message
    void LinkLayerComSendMemoryData();
    void LinkLayerComSendApplyData();

    //////////////////////////////////////////////////////////////////////////
    // Layer Into Moment
    void LinkLayerCircleMomentToBegin();
    void LinkLayerCircleMomentToCircle();
    void LinkLayerCircleMomentToApply();
    void LinkLayerCircleMomentToDrift();

    //////////////////////////////////////////////////////////////////////////
    // Do with the Stage's Message Data
    // Do With the Out Time
    void LinkLayerChainDealWithOuttime(int& nid);
    // Do With Receive the Stage's Message
    void LinkLayerChainDealWithRecvMsg(int& nid);

    void RecordInChainPosition(CSCModelChain& nChain,int nId);

public:
    // Parameter
    bool	m_bMsgStill;            // Message Still
    int		m_nTNotInChainCt;		// 不在链表中的时间汇总
    int		m_nTNotInChainCtLmt;	// 不在链接中的时间界限
    int		m_IsInChainWhereNum;	// 在链表中的第几个 0 表示不在链表中
};

#endif // CSC_01LAYERCLIENT_H

#ifndef CSC_01LAYERHEAD_H
#define CSC_01LAYERHEAD_H

#include "VHFLayer/SC_01Layer.h"

class CSC_01LayerHead : public CSC_01Layer
{
public:
    CSC_01LayerHead();
    virtual ~CSC_01LayerHead();

    //////////////////////////////////////////////////////////////////////////
    //	User Layer Control
    //////////////////////////////////////////////////////////////////////////
    // Interface to User,Set the Stage & Layer Information
    // Input the Layer Information
    void UseDataInput(char* pchar,const int nlength);

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
    // Do with the Stage Apply
    void LinkLayerChainDealWithStageApply(int& nid,int& napply);
    // Do With the Out Time
    void LinkLayerChainDealWithOuttime(int& nid);
    // Do With Receive the Stage's Message
    void LinkLayerChainDealWithRecvMsg(int& nid);

    //////////////////////////////////////////////////////////////////////////
    // Update the List State
    //////////////////////////////////////////////////////////////////////////
    // Set the State Receive Data :: LAYSTA_SUCCESS
    void LinkLayerChainSetStageRecvOk(int& nid);
    // Set the Stage Out Time :: LAYSTA_OFFLINE
    void LinkLayerChainSetStageOuttime(int& nid);

    // Layer Control
    void LinkLayerChainMainChange(int nmain);

private:
    //////////////////////////////////////////////////////////////////////////
    // Analyze Data Layer Char
    //////////////////////////////////////////////////////////////////////////
    // Analyze the Sentence
    void ActAnalyzeVHFLayer(char cRec);
    void ActAnalyzeError();

private:
    //////////////////////////////////////////////////////////////////////////
    // Parameter
    // Chain Message Save Space
    char*               m_pDataControl;		// Control Message Save
    int                 m_pDataCtlLen;		// Control Message length

public:
    //////////////////////////////////////////////////////////////////////////
    // Chain Member is Change
    bool                m_bChainChange;     // Chain is or Not Change
    CSCModelChain       m_nChainTmp;        // Temp Chain for Save Change Information
    bool                m_bMainChange;		// Main is or Not Change
    int                 m_bMainCode;		// Change Main Code
};

#endif // CSC_01LAYERHEAD_H

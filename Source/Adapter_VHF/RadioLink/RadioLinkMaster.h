#ifndef RADIOLINKMASTER_H
#define RADIOLINKMASTER_H

#include "LinkCommon.h"
#include "RadioLink.h"

class RadioLinkMaster : public RadioLink
{
public:
    RadioLinkMaster();
    virtual ~RadioLinkMaster();
    //////////////////////////////////////////////////////////////////////////
    // Link Layer Circle
    // Main Control
    virtual void LinkLayerMainCircle();
    //////////////////////////////////////////////////////////////////////////
    // Layer Into Moment
    virtual void LinkLayerCircleMomentToBegin();
    virtual void LinkLayerCircleMomentToCircle();
    virtual void LinkLayerCircleMomentToApply();
    virtual void LinkLayerCircleMomentToDrift();

    virtual void timerProcess();

protected:
    virtual void recvDataAnalyze(ObjMsg &recvMsg);

private:
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

    // Send Message
    void LinkLayerComSendMemoryData();
    void LinkLayerComSendApplyData();

private:
    // Parameter
    // Chain Message Save Space
    char*               m_pDataControl;		// Control Message Save
    int                 m_pDataCtlLen;		// Control Message length
    //////////////////////////////////////////////////////////////////////////
    // Chain Member is Change
    bool                m_bChainChange;     // Chain is or Not Change
    ModelChain          m_nChainTmp;        // Temp Chain for Save Change Information
    bool                m_bMainChange;		// Main is or Not Change
    int                 m_bMainCode;		// Change Main Code

};

#endif // RADIOLINKMASTER_H

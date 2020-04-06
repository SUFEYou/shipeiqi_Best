#ifndef RADIOLINKCLIENT_H
#define RADIOLINKCLIENT_H

#include "RadioLink/LinkCommon.h"
#include "RadioLink/RadioLink.h"

class RadioLinkClient : public RadioLink
{
public:
    RadioLinkClient();
    virtual ~RadioLinkClient();

    inline int getNotInChainCt() { return m_nTNotInChainCt; }
    inline void setNotInChainCt(int x) { m_nTNotInChainCt = x; }

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
    // Send Message
    void LinkLayerComSendMemoryData();
    void LinkLayerComSendApplyData();

    // Do with the Stage's Message Data
    // Do With the Out Time
    void LinkLayerChainDealWithOuttime(int& nid);
    // Do With Receive the Stage's Message
    void LinkLayerChainDealWithRecvMsg(int& nid);

    void RecordInChainPosition(ModelChain& nChain,int nId);

private:
    // Parameter
    bool	m_bMsgStill;            // Message Still
    int		m_nTNotInChainCt;		// 不在链表中的时间汇总
    int		m_nTNotInChainCtLmt;	// 不在链接中的时间界限
    int		m_IsInChainWhereNum;	// 在链表中的第几个 0 表示不在链表中

};

#endif // RADIOLINKCLIENT_H

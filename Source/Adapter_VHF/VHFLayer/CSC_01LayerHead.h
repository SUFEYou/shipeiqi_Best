#ifndef CSC_01LAYERHEAD_H
#define CSC_01LAYERHEAD_H

#include "VHFLayer/SC_01Layer.h"

class CSC_01LayerHead : public CSC_01Layer
{
public:
    CSC_01LayerHead();
    virtual ~CSC_01LayerHead();


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

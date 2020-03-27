#include "CSC_01LayerHead.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSC_01LayerHead::CSC_01LayerHead()
{
    m_pDataControl	= new char[512];// Control Message Save
    m_pDataCtlLen	= 0;			// Control Message length

    m_nDataMaxLen = PACK_LENGTHLIMIT;

    //////////////////////////////////////////////////////////////////////////
    // Chain Member is Change
    m_bChainChange	= false;		// Chain is or Not Change
    m_bMainChange	= false;
    m_bMainCode	= 0;

}

CSC_01LayerHead::~CSC_01LayerHead()
{
    if (m_pDataControl)
    {
        delete []m_pDataControl;
        m_pDataControl = NULL;
    }
}

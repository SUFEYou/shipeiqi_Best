#ifndef CE_VHFNODEMANAGE_H
#define CE_VHFNODEMANAGE_H

class CSC_01LayerHead;
class CSC_01LayerClient;

class CE_VHFNodeManage
{
public:
    CE_VHFNodeManage();
    ~CE_VHFNodeManage();


public:
    CSC_01LayerHead*            m_pLayerVHFHead;	// VHF Layer head
    CSC_01LayerClient*          m_pLayerVHFClient;	// VHF Layer Client
};

#endif // CE_VHFNODEMANAGE_H

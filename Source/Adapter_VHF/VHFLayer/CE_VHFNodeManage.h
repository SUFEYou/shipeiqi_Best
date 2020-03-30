#ifndef CE_VHFNODEMANAGE_H
#define CE_VHFNODEMANAGE_H

#include <QObject>
#include <QMutex>

class CSC_01LayerHead;
class CSC_01LayerClient;
class QTimer;

class CE_VHFNodeManage : public QObject
{
    Q_OBJECT
public:
    static CE_VHFNodeManage* getInstance();
    void init();

    //////////////////////////////////////////////////////////////////////////
    // 通信串口操作
    //////////////////////////////////////////////////////////////////////////
    // 数据输出
    bool PortCommSendOutData(char* pChar,int nLen);




    void VHFLayerChangeClientToHead();

private:
    CE_VHFNodeManage();
    ~CE_VHFNodeManage();

signals:
    void comSendData(char* pChar,int nLen);

private slots:
    void OnCommRecData(const QByteArray &data);

    void dealVHFLayerTimer();

public:
    static CE_VHFNodeManage*    m_instance;
    static QMutex               m_mutex;

    CSC_01LayerHead*            m_pLayerVHFHead;	// VHF Layer head
    CSC_01LayerClient*          m_pLayerVHFClient;	// VHF Layer Client

    QTimer*                     m_VHFLayerTimer;
};

#endif // CE_VHFNODEMANAGE_H

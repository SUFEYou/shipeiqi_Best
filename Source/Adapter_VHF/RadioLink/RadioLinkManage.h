#ifndef RADIOLINKMANAGE_H
#define RADIOLINKMANAGE_H

#include <QMutex>

class RadioLinkMaster;
class RadioLinkClient;

class RadioLinkManage
{
public:
    static RadioLinkManage* getInstance();
    //////////////////////////////////////////////////////////////////////////
    // 通信串口操作
    //////////////////////////////////////////////////////////////////////////
    // 数据输出
    bool PortCommSendOutData(char* pChar, int nLen);
    void OnCommRecData(const QByteArray &data);

private:
    RadioLinkManage();
    ~RadioLinkManage();

private:
    static RadioLinkManage*         m_instance;
    static QMutex                   m_mutex;

    RadioLinkMaster*                m_radioLinkMaster;
    RadioLinkClient*                m_radioLinkClient;
};

#endif // RADIOLINKMANAGE_H

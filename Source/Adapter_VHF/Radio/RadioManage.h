#ifndef RADIOMANAGE_H
#define RADIOMANAGE_H

#include <QObject>
#include <QMutex>
#include <stdint.h>
#include "Radio/RadioType.h"

class Radio;



class RadioManage : public QObject
{
    Q_OBJECT
public:
    static RadioManage* getInstance();
    void init();

    void writeCtrlData(uint16_t funCode, char* pChar,int nLen);
    void onCtrlAck(uint16_t funCode, char* pChar,int nLen);

    void writeLinkData(char* pChar,int nLen);  
    void onRecvLinkData(QByteArray data);
    void onRecvLinkData(const char* data, const uint16_t len);

private:
    RadioManage();
    ~RadioManage();

private:
    static RadioManage       *m_instance;
    static QMutex            m_mutex;

    int                      curRadioTyp;
    Radio                    *radio;


};

#endif // RADIOMANAGE_H

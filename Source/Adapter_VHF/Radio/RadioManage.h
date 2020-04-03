#ifndef RADIOMANAGE_H
#define RADIOMANAGE_H

#include <QObject>
#include <QMutex>
#include "Radio/Radio181d.h"
#include "Radio/Radio171al.h"
#include "Radio/Radio212.h"
#include <stdint.h>

#pragma pack(1)
typedef enum Radio_Type	{
    RADIO_181D	= 1814,                 //
    RADIO_171D	= 1714,
    RADIO_171AL	= 1716,

}RadioTyp;



class RadioManage : public QObject
{
    Q_OBJECT
public:
    static RadioManage* getInstance();
    void init();

    void writeCtrlData(uint16_t ctrlTyp, char* pChar,int nLen);
    void onCtrlAck(uint16_t ackTyp, char* pChar,int nLen);

    void writeLinkData(char* pChar,int nLen);  
    void onRecvLinkData(QByteArray data);

private:
    RadioManage();
    ~RadioManage();

private:
    static RadioManage       *m_instance;
    static QMutex            m_mutex;

    int                      curRadioTyp;
    Radio181D                *radio181D;
    Radio171AL               *radio171AL;
    Radio212                 *radio212;

};

#endif // RADIOMANAGE_H

#ifndef RADIOMANAGE_H
#define RADIOMANAGE_H

#include <QObject>
#include <QMutex>
#include "Radio/Radio181d.h"
#include "Radio/Radio171al.h"
#include "Radio/Radio171d.h"
#include "Radio/Radio781tcp.h"
#include "Radio/Radio212tcr.h"
#include <stdint.h>

#pragma pack(1)
typedef enum Radio_Type	{
    RADIO_181D	= 1814,                 //
    RADIO_171D	= 1714,
    RADIO_171AL	= 1716,
    RADIO_781TCP= 7814,
    RADIO_212TCR= 2124,
    RADIO_220   = 2204,
    RADIO_230   = 2304,

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
    Radio171D                *radio171D;
    Radio171AL               *radio171AL;
    Radio781TCP              *radio781TCP;
    Radio212TCR              *radio212TCR;

};

#endif // RADIOMANAGE_H

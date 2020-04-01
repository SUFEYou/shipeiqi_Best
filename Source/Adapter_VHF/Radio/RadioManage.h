#ifndef RADIOMANAGE_H
#define RADIOMANAGE_H

#include <QObject>
#include <QMutex>
#include "Radio/Radio181d.h"
#include "Radio/Radio171al.h"
#include "Radio/Radio212.h"

typedef enum Radio_Type	{
    RADIO_181D	= 1814,                 //
    RADIO_171D	= 1714,
    RADIO_171AL	= 1716,

}RadioTyp;

typedef enum Radio_Ctrl	{
    Channel_Set	 = 0x01,                 //
    SND_Freq_Set = 0x02,
    REV_Freq_Set = 0x03,
    Work_Mod_Set = 0x04,
    Work_Typ_Set = 0x05,
    Sque_Lev_Set = 0x06,
    Powr_Lev_Set = 0x07
}RadioCtrl;



class RadioManage : public QObject
{
    Q_OBJECT
public:
    static RadioManage* getInstance();
    void init();
    void writeCtrlData(char* pChar,int nLen);
    void writeLinkData(char* pChar,int nLen);

    void onResponseCtrl(char* pChar,int nLen);
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

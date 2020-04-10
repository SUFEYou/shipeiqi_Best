#ifndef RADIO212TCR_H
#define RADIO212TCR_H

#include <QObject>
#include <QMutex>
#include <stdint.h>
#include "Uart/qextserialport.h"
#include "socket/socketcommon.h"

class Radio212TCR: public QObject
{
    Q_OBJECT

public:
    Radio212TCR();
    ~Radio212TCR();
    void serialInit();
    int writeCtrlData(uint16_t ctrlTyp, char* data, int len);
    int writeLinkData(char* data, int len);

    inline VHF_ACK_STATE getRadioState() const { return radioState; }

private:
    void wConverte(char* srcData, int srcLen, char* dstData, int &dstLen);
    void updateRadioState(char* data, int len);

private slots:
    void readDataCom();
    void readCtrlCom();
    void onTimer();


private:
    VHF_ACK_STATE           radioState;

};

#endif // RADIO212TCR_H

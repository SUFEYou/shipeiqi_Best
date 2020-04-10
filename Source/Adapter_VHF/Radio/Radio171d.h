#ifndef RADIO171D_H
#define RADIO171D_H

#include <QObject>
#include <QMutex>
#include <stdint.h>
#include <QTimer>
#include "Uart/qextserialport.h"
#include "socket/socketcommon.h"

class Radio171D: public QObject
{
    Q_OBJECT

public:
    Radio171D();
    ~Radio171D();
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

#endif // RADIO171D_H

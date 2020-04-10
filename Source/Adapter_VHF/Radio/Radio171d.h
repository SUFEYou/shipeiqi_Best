#ifndef RADIO171D_H
#define RADIO171D_H

#include "Radio.h"

class Radio171D : public Radio
{
    Q_OBJECT

public:
    Radio171D();
    virtual ~Radio171D();
    virtual void serialInit();
    virtual int writeCtrlData(uint16_t ctrlTyp, char* data, int len);
    virtual int writeLinkData(char* data, int len);

private slots:
    void readCom();
    void onTimer();


private:
    uint16_t getCRC(unsigned char* buf, unsigned int len);
    void wConverte(char* srcData, int srcLen, char* dstData, int &dstLen);
    void updateRadioState(char* data, int len);

};

#endif // RADIO171D_H

#ifndef RADIO212TCR_H
#define RADIO212TCR_H

#include "Radio.h"

class Radio212TCR : public Radio
{
    Q_OBJECT

public:
    Radio212TCR();
    virtual ~Radio212TCR();
    virtual void serialInit();
    virtual int writeCtrlData(uint16_t ctrlTyp, char* data, int len);
    virtual int writeLinkData(char* data, int len);

private:
    void wConverte(char* srcData, int srcLen, char* dstData, int &dstLen);
    void updateRadioState(char* data, int len);

private slots:
    void readDataCom();
    void readCtrlCom();
    void onTimer();


private:

};

#endif // RADIO212TCR_H

#ifndef RADIO781TCP_H
#define RADIO781TCP_H

#include "Radio.h"

class Radio781TCP : public Radio
{
    Q_OBJECT

public:
    Radio781TCP();
    virtual ~Radio781TCP();
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
    QTimer                  *timer;
    long                    updTim;

};

#endif // RADIO781TCP_H

#ifndef RADIO181D_H
#define RADIO181D_H

#include "Radio.h"

class Radio181D : public Radio
{
    Q_OBJECT

public:
    Radio181D();
    virtual ~Radio181D();
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
    //////////////////////////////////////////////////////////
    int                     Protocol;           //0:非集群协议   1:集群协议
    int                     sChannel;           //初始频道
    //////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////
    QTimer                  *timer;
    long                    updTim;
    //////////////////////////////////////////////////////////


};

#endif // RADIO181D_H

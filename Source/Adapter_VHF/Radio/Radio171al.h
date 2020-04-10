#ifndef RADIO171AL_H
#define RADIO171AL_H

#include "Radio.h"
#include <QList>

class Radio171AL : public Radio
{
    Q_OBJECT

public:
    Radio171AL();
    virtual ~Radio171AL();
    virtual void serialInit();
    virtual int writeCtrlData(uint16_t ctrlTyp, char* data, int len);
    virtual int writeLinkData(char* data, int len);

private slots:
    void readCom();
    void onTimer();

private:
    void recvDataSubpackage();
    void recvDataParse();
    uint16_t getCRC(unsigned char* buf, unsigned int len);
    void rConverte(const char* srcData, const int srcLen, char* dstData, int &dstLen);
    void wConverte(char* srcData, int srcLen, char* dstData, int &dstLen);
    void updateRadioState(char* data, int len);

private:
    QList<QByteArray>                   m_recvDataList;

};

#endif // RADIO171AL_H

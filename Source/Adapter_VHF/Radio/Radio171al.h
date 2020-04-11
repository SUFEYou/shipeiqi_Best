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
    void rConverte(const char* srcData, const int srcLen, char* dstData, int& dstLen);
    void sendDataPackage(uint16_t type, const char* data, const int len);
    void wConverte(const char* srcData, const int srcLen, char* dstData, int& dstLen);
    void updateRadioState(uint16_t type, const char* data, const int len);
    uint16_t getCRC(unsigned char* buf, unsigned int len);

private:
    QList<QByteArray>                   m_recvDataList;
    QTimer                              *m_timer;

};

#endif // RADIO171AL_H

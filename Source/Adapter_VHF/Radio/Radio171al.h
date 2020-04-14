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
    virtual int writeCtrlData(uint16_t funCode, char* data, int len);
    virtual int writeLinkData(char* data, int len);

private slots:
    void readCom();
    void onTimer();


private:
    void packageData();
    void parseData();
    void decode(const char* srcData, const int srcLen, char* dstData, int& dstLen);
    void writeData(uint16_t type, const char* data, const int len);
    void enCode(const char* srcData, const int srcLen, char* dstData, int& dstLen);
    void updateRadioState(uint16_t type, const char* data, const int len);
    uint16_t getCRC(unsigned char* buf, unsigned int len);

private:
    QMutex                   m_dataMutex;
    QextSerialPort           *dataCom;

    QList<QByteArray>        m_recvDataList;
    QTimer                   *m_timer;
};

#endif // RADIO171AL_H

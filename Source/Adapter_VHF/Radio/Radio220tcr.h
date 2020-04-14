#ifndef RADIO220TCR_H
#define RADIO220TCR_H

#include "Radio.h"

class Radio220tcr : public Radio
{
    Q_OBJECT
public:
    Radio220tcr();
    virtual ~Radio220tcr();
    virtual void serialInit();
    virtual int writeCtrlData(uint16_t ctrlTyp, char* data, int len);
    virtual int writeLinkData(char* data, int len);

private:
    void packageData();
    void parseData();
    void messageSeparate(const char* data, const int len);
    void decode(const char* srcData, const int srcLen, char* dstData, int& dstLen);
    void writeData(char nDimID, char type, const char* data, const int len);
    void enCode(const char* srcData, const int srcLen, char* dstData, int& dstLen);
    void updateRadioState(uint16_t type, const char* data, const int len);
    char getCRC(const char* data, const quint16 len);



private slots:
    void readCom();
    void onTimer();

private:
    QMutex                              m_dataMutex;
    QextSerialPort                      *dataCom;

    QList<QByteArray>                   m_recvDataList;
    QTimer                              *timer;
    long                                updTim;
    uint16_t                            m_nModemState;
};

#endif // RADIO220TCR_H

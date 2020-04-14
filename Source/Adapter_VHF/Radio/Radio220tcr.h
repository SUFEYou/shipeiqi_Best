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
    void recvDataSubpackage();
    void recvDataParse();
    void messageSeparate(const char* data, const int len);
    void rConverte(const char* srcData, const int srcLen, char* dstData, int& dstLen);
    void sendDataPackage(char nDimID, char type, const char* data, const int len);
    void wConverte(char* srcData, int srcLen, char* dstData, int& dstLen);
    char CRCVerify(const char* data, const quint16 len);
    void updateRadioState(char* data, int len);

private slots:
    void readCom();
    void onTimer();

private:
    QList<QByteArray>                   m_recvDataList;
    QTimer                              *timer;
    long                                updTim;
    uint16_t                            m_nModemState;
};

#endif // RADIO220TCR_H

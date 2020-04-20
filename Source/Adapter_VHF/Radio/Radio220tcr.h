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
    virtual int writeCtrlData(uint16_t funCode, char* data, int len);
    virtual int writeLinkData(char* data, int len);

private:
    void packageData();
    void parseData();
    void updateRadioState(const char* data, const int len);
    void decode(const char* srcData, const int srcLen, char* dstData, int& dstLen);
    void writeData(char nDimID, char type, const char* data, const int len);
    void enCode(const char* srcData, const int srcLen, char* dstData, int& dstLen);
    char getCRC(const char* data, const quint16 len);

    void radioModemCanSendData(bool isSend);

    void setModemNoticeSendEnd();
    void setModemNoticeCancel();

    void setWorkTyp(const uint8_t nWorkTyp);
    void setWorkMod(const uint8_t nWorkMod);
    void setChannel(const uint16_t nCHN);
    void setPower(const uint8_t nPower);
    void setSquelch(const uint8_t nSquelch);
    void setTxFreq(const uint64_t nTxFreq);
    void setRxFreq(const uint64_t nRxFreq);

    void checkDisconnect();


private slots:
    void readCom();
    void onTimer();

private:
    QMutex                              m_dataMutex;
    QextSerialPort                      *dataCom;

    QList<QByteArray>                   m_recvDataList;
    QTimer                              *timer;
    long                                updTim;

    uint16_t                            m_nRadioState;                  // Radio State

    bool                                m_nModemSend;
    uint16_t                            m_nModemState;
    uint16_t                            m_SendingTimesCount;

    char*                               m_DycArrayData;
    uint16_t                            m_DycArrayLen;

    uint8_t                             m_RequestCount;
};

#endif // RADIO220TCR_H

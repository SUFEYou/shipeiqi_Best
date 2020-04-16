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

    void setChannel(const uint16_t nCHN);
    void setPower(const uint8_t nPower);


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

#endif // RADIO212TCR_H

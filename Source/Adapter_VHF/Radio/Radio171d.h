#ifndef RADIO171D_H
#define RADIO171D_H

#include "Radio.h"

class Radio171D : public Radio
{
    Q_OBJECT

public:
    Radio171D();
    virtual ~Radio171D();
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
    void updateRadioState(uint16_t type, char* data, const int len);
    uint16_t getCRC(unsigned char* buf, unsigned int len);
    void setChannelParam(char* data, const int len);

    void bcd2uint8(uint8_t src, uint8_t* dst);
    void uint82bcd(uint8_t src, uint8_t* dst);

private:
    QMutex                      m_dataMutex;
    QextSerialPort              *dataCom;

    QList<QByteArray>           m_recvDataList;
    QTimer                      *m_timer;

    bool                        m_connected;
    uint8_t                     m_disconnectCnt;
    bool                        m_workModFlag;
    uint8_t                     m_workMod;          //工作模式
    bool                        m_freqFlag;
    uint64_t                    m_freq;             //频率
    bool                        m_squelchFlag;
    uint8_t                     m_squelch;          //静噪
};

#endif // RADIO171D_H

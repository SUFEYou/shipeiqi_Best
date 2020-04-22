#ifndef RADIO230_H
#define RADIO230_H

#include "Radio.h"

class Radio230: public Radio
{
    Q_OBJECT
public:
    Radio230();
    virtual ~Radio230();
    virtual void serialInit();
    virtual int writeCtrlData(uint16_t funCode, char* data, int len);
    virtual int writeLinkData(char* data, int len);

private:
    void packageData();
    void parseData(const unsigned char nDeviceAddr, const QByteArray array);

private slots:
    void readDataCom();
    void readCtrlCom();
    void onTimer();

private:
    QMutex                              m_ctrlMutex;
    QextSerialPort                      *ctrlCom;

    QMutex                              m_dataMutex;
    QextSerialPort                      *dataCom;

    QTimer                              *timer;
    long                                updTim;
};

#endif // RADIO230_H

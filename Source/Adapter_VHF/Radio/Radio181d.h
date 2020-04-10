#ifndef RADIO181D_H
#define RADIO181D_H

#include <QObject>
#include <QMutex>
#include <stdint.h>
#include <QTimer>
#include "Uart/qextserialport.h"
#include "socket/socketcommon.h"

class Radio181D: public QObject
{
    Q_OBJECT

public:
    Radio181D();
    ~Radio181D();
    void serialInit();
    int writeCtrlData(uint16_t ctrlTyp, char* data, int len);
    int writeLinkData(char* data, int len);

    inline VHF_ACK_STATE getRadioState() const { return radioState; }

private:
    void wConverte(char* srcData, int srcLen, char* dstData, int &dstLen);
    void updateRadioState(char* data, int len);

private slots:
    void readDataCom();
    void readCtrlCom();
    void onTimer();

private:
    QMutex                   m_ctrlMutex;
    QextSerialPort          *ctrlCom;

    QMutex                   m_dataMutex;
    QextSerialPort          *dataCom;

    QByteArray              dataArray;

    //////////////////////////////////////////////////////////
    int                     Protocol;           //0:非集群协议   1:集群协议
    int                     sChannel;           //初始频道
    //////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////
    QTimer                  *timer;    
    VHF_ACK_STATE           radioState;
    long                    updTim;
    //////////////////////////////////////////////////////////


};

#endif // RADIO181D_H

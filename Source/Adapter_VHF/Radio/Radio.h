#ifndef RADIO_H
#define RADIO_H

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <stdint.h>
#include "RadioManage.h"
#include "Uart/qextserialport.h"
#include "socket/socketcommon.h"

class Radio : public QObject
{
    Q_OBJECT
public:
    Radio();
    virtual ~Radio();

    virtual void serialInit() = 0;
    virtual int writeCtrlData(uint16_t ctrlTyp, char* data, int len) = 0;
    virtual int writeLinkData(char* data, int len) = 0;

    inline VHF_ACK_STATE getRadioState() const { return radioState; }


protected:
    QMutex                   m_ctrlMutex;
    QextSerialPort          *ctrlCom;

    QMutex                   m_dataMutex;
    QextSerialPort          *dataCom;

    VHF_ACK_STATE           radioState;

    QByteArray              dataArray;

private:

};

#endif // RADIO_H

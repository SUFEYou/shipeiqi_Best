#ifndef RADIO171AL_H
#define RADIO171AL_H

#include <QObject>
#include <QMutex>
#include <stdint.h>
#include "Uart/qextserialport.h"

class Radio171AL: public QObject
{
    Q_OBJECT

public:
    Radio171AL();
    ~Radio171AL();
    void serialInit();
    int writeCtrlData(uint16_t ctrlTyp, char* data, int len);
    int writeLinkData(char* data, int len);

private slots:
    void readCom();

private:
    QextSerialPort       *com;

};

#endif // RADIO171AL_H

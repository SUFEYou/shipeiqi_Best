#ifndef RADIO171AL_H
#define RADIO171AL_H

#include <QObject>
#include <QMutex>
#include "Uart/qextserialport.h"

class Radio171AL: public QObject
{
    Q_OBJECT

public:
    Radio171AL();
    ~Radio171AL();
    void serialInit();
    int ctrlRadio(char* data, int len);
    int sendData(char* data, int len);

private slots:
    void readCom();

private:
    QextSerialPort       *com;

};

#endif // RADIO171AL_H

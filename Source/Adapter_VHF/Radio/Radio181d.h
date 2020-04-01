#ifndef RADIO181D_H
#define RADIO181D_H

#include <QObject>
#include <QMutex>
#include "Uart/qextserialport.h"

class Radio181D: public QObject
{
    Q_OBJECT

public:
    Radio181D();
    ~Radio181D();
    void serialInit();
    int writeCtrlData(char* data, int len);
    int writeLinkData(char* data, int len);

private slots:
    void readDataCom();
    void readCtrlCom();

private:
    QMutex                   m_ctrlMutex;
    QextSerialPort          *ctrlCom;

    QMutex                   m_dataMutex;
    QextSerialPort          *dataCom;


};

#endif // RADIO181D_H

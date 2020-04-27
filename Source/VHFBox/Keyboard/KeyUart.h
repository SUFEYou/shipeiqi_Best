#ifndef KEYUART_H
#define KEYUART_H

#include <QObject>
#include <QMutex>
#include <stdint.h>
#include <QTimer>
#include "Uart/qextserialport.h"

class KeyUart: public QObject
{
    Q_OBJECT
public:
    static KeyUart* getInstance();
    void serialInit();

private:
    KeyUart();
    ~KeyUart();

    void onPttOn();
    void onPttOff();
    void onKey(char key);


private slots:

    void readDataCom();
    void onTimer();

private:
    static KeyUart         *m_instance;
    static QMutex           m_getMutex;

    QMutex                   m_dataMutex;
    QextSerialPort          *dataCom;

    int                     toChangeCount;      //切换按键（暂定【下】按键）的连续个数
    int                     toChangeTime;       //切换按键（暂定【下】按键）的连续时间

    int                     pttOnTim;
    QTimer                  *timer;


};

#endif // KEYUART_H

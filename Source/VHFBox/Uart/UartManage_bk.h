#ifndef UARTMANAGE_H
#define UARTMANAGE_H

#include <QObject>
#include <QMutex>

class QextSerialPort;

class UartManage : public QObject
{
    Q_OBJECT
public:
    static UartManage* getInstance();
    void init();

private:
    UartManage(QObject *parent = NULL);
    ~UartManage();
    void serialInit();

private slots:
    void readMyCom();

private:

    static UartManage       *m_instance;
    static QMutex           m_mutex;

    QextSerialPort          *m_myCom;

};

#endif // UARTMANAGE_H

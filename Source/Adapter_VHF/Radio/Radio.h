#ifndef RADIO_H
#define RADIO_H

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <stdint.h>
#include "RadioManage.h"
#include "Uart/qextserialport.h"
#include "socket/SocketCommu.h"

#define MAXDATALENGTH 4096

class Radio : public QObject
{
    Q_OBJECT
public:
    Radio();
    virtual ~Radio();

    virtual void serialInit() = 0;
    virtual int writeCtrlData(uint16_t funCode, char* data, int len) = 0;
    virtual int writeLinkData(char* data, int len) = 0;

    inline RADIO_STATE getRadioState() const { return radioState; }


protected:

    RADIO_STATE             radioState;
    QByteArray              dataArray;

private:

};

#endif // RADIO_H

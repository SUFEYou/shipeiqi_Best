#ifndef UDPREVCAST_H
#define UDPREVCAST_H

#include <QObject>
#include <stdint.h>
#include <QUdpSocket>
#include <QAbstractSocket>
#include "Socket/SocketCommu.h"

#pragma pack(1)
struct LocationInfo{
    int16_t     LocaTyp;
    double      Lat;
    double      Lon;
    uint16_t    Year;
    uint16_t    Month;
    uint16_t    Day;
    uint16_t    Hour;
    uint16_t    Min;
    uint16_t    Sec;
};
#pragma pack()


class UdpRevCast: public QObject
{
    Q_OBJECT
public:
    UdpRevCast();

    void init(int recivPort);

private:
//    void separateData(const char* data, const uint16_t len);
//    void parseData();

private slots:
    void onRev();
    void onError(QAbstractSocket::SocketError socketError);

private:

    QUdpSocket                  *m_udpSocket;
    quint16                     m_recivPort;

//    QByteArray                  recvArray;
//    QList<QByteArray>           recvList;
//    LocationInfo                locationInfo;
};

#endif // UDPREVCAST_H

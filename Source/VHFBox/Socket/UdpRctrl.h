#ifndef UDPRCTL_H
#define UDPRCTL_H

#include <stdint.h>
#include <QUdpSocket>
#include <QAbstractSocket>
#include "Socket/SocketCommu.h"

class UDPRctrl: public QObject
{
    Q_OBJECT
public:
    UDPRctrl();

    void init(int recivPort, QString sndToIP, int sndToPort);
    void registSocket();
    void sendRadioCtrl(int setTyp, uint64_t value);

    inline int getIndex() const { return index; }
    inline int getDevID() const { return devID; }
    inline int getRadioTyp()    const { return radioTyp; }
    inline quint8 getPriority() const { return priority; }

    inline void setIndex(int value) { index = value; }
    inline void setDevID(int value) { devID = value; }
    inline void setRadioTyp(int value)    { radioTyp = value; }
    inline void setPriority(quint8 value) { priority = value; }

private:

private slots:
    void onRev();
    void onError(QAbstractSocket::SocketError socketError);

private:
    int                 index;
    int                 devID;
    int                 radioTyp;

    QUdpSocket          *m_udpSocket;
    QHostAddress        m_sndToIP;
    quint16             m_recivPort;
    quint16             m_sndToPort;

    quint8              priority;

};

#endif // UDPRCTL_H

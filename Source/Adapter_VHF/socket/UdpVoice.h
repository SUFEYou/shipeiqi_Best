#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QUdpSocket>
#include <QAbstractSocket>
#include <stdint.h>
#include "socket/SocketCommu.h"
#include "Audio/AudioControl.h"

class UDPVoice: public QObject
{
    Q_OBJECT
public:
    UDPVoice();
    void init(int port);
    void sendData(char* pData,int nLen);
    void sendVoiceData(AudioData audioData);

private:
    void registVoice(QString sessionKey, VOICE_REGIST_VO regPipe);
    int getRegistedID(QString sessionKey);
    void sendRegistState(uint8_t regState, QHostAddress netAddr, uint32_t netPort);

private slots:
    void onRev();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QUdpSocket          *m_udpSocket;
    quint16             m_Port;

    uint8_t             FrameSN;

    VOICE_REGIST_VO     regArray[4];

};

#endif // UDPSOCKET_H

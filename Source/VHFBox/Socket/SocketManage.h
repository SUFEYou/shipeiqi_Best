#ifndef SOCKETMANAGE_H
#define SOCKETMANAGE_H

#include <QMutex>
#include "Socket/UdpVoice.h"
#include "Socket/UdpRctrl.h"
#include <qnetworkinterface.h>
#include "Audio/AudioControl.h"
#include "Socket/UdpRevCast.h"

class SocketManage
{
public:
    static SocketManage* getInstance();
    bool init();


    UDPRctrl *getCtrlUdp1() const { return rctrlUdp1; }
    UDPVoice *getVoicUdp1() const { return voiceUdp1; }

    UDPRctrl *getCtrlUdp2() const { return rctrlUdp2; }
    UDPVoice *getVoicUdp2() const { return voiceUdp2; }

    UDPRctrl *getCtrlUdp3() const { return rctrlUdp3; }
    UDPVoice *getVoicUdp3() const { return voiceUdp3; }

    UDPRctrl *getCtrlUdp4() const { return rctrlUdp4; }
    UDPVoice *getVoicUdp4() const { return voiceUdp4; }

    UDPVoice *getVoicUdp(int ID);
    UDPRctrl *getCtrlUdp(int ID);

    void registApplySocket(int ID);
    void sendVoiceData(AudioData audioData);
    void sendPtt(bool pttOn);

    QString getLocalIPAddr();


    bool getPttOn() { return pttOn; }
    void setPttOn(bool value);

private:
    SocketManage();
    ~SocketManage();

private:
    static SocketManage    *m_instance;
    static QMutex          m_Mutex;

    UDPRctrl*      rctrlUdp1;
    UDPVoice*      voiceUdp1;

    UDPRctrl*      rctrlUdp2;
    UDPVoice*      voiceUdp2;

    UDPRctrl*      rctrlUdp3;
    UDPVoice*      voiceUdp3;

    UDPRctrl*      rctrlUdp4;
    UDPVoice*      voiceUdp4;

    UdpRevCast*    posRevUdp;

    bool           pttOn;

};

#endif // SOCKETMANAGE_H

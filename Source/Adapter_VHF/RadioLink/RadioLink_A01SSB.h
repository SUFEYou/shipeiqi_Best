#ifndef RADIOLINK_A01SSB_H
#define RADIOLINK_A01SSB_H

class RadioLink_A01SSB
{
public:
    RadioLink_A01SSB();
    ~RadioLink_A01SSB();

    void SetAvailable(bool available);
    bool GetAvailable();
    void recvData(const char* pchar,const int nlength);
    void timerProcess();

private:
    bool        m_bAvailable;
};

#endif // RADIOLINK_A01SSB_H

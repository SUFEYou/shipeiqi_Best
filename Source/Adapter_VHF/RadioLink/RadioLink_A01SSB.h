#ifndef RADIOLINK_A01SSB_H
#define RADIOLINK_A01SSB_H

class RadioLink_A01SSB
{
public:
    RadioLink_A01SSB();
    ~RadioLink_A01SSB();

    void SetAvailable(bool available);
    bool GetAvailable();
    void recvData(char* pchar,const int nlength);
    void timerProcess();
    void setCodeMe(int x);

private:
    void packageData(const char type, const int sendid, const int recvid, const int serial, const char* data, const int datalen);

private:
    bool        m_bAvailable;
    int         m_nCodeMe;		// My Stage Code
};

#endif // RADIOLINK_A01SSB_H

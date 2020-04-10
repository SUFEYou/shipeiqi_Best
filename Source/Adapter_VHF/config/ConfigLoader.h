#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <QObject>
#include <QMutex>
#include <stdint.h>

class ConfigLoader
{
public:
    static ConfigLoader* getInstance();
    bool load();

    uint16_t getProgramType() const;
    uint16_t getProgramID()   const;
    uint32_t getRadioTyp()    const;
    uint32_t getRadioID()     const;


    inline int getCtrlPort() const { return ctrlPort; }
    inline int getVoicPort() const { return voicPort; }

    inline QString getTcpIP()  const { return TcpIP; }
    inline int getTcpPort()    const { return TcpPort; }


private:
    ConfigLoader();
    ~ConfigLoader();

    void createConfig();
    void loadConfig();


private:
    static ConfigLoader    *m_instance;
    static QMutex          m_Mutex;

    uint16_t   programType;
    uint16_t   programID;
    uint32_t   radioTyp;
    uint32_t   radioID;

    int         ctrlPort;
    int         voicPort;

    //与通信平台通信配置信息
    QString     TcpIP;
    int         TcpPort;


};

#endif // CONFIGLOADER_H

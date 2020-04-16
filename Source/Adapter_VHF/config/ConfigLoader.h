#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <QObject>
#include <QMutex>
#include <stdint.h>
#include "ConfigRadio.h"
#include "ConfigRadio171al.h"
#include "ConfigRadio171d.h"
#include "ConfigRadio181d.h"
#include "ConfigRadio781tcp.h"
#include "ConfigRadio212tcr.h"
#include "ConfigRadio220.h"

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

    inline ConfigRadio* getConfigRadio() { return m_radio; }

private:
    ConfigLoader();
    ~ConfigLoader();

    void createConfig();
    void loadConfig();
    void loadRadioConfig(uint32_t tRadioTyp);


private:
    static ConfigLoader     *m_instance;
    static QMutex           m_Mutex;

    uint16_t                programType;
    uint16_t                programID;
    uint32_t                radioTyp;
    uint32_t                radioID;

    int                     ctrlPort;
    int                     voicPort;

    //与通信平台通信配置信息
    QString                 TcpIP;
    int                     TcpPort;

    ConfigRadio             *m_radio;
};

#endif // CONFIGLOADER_H

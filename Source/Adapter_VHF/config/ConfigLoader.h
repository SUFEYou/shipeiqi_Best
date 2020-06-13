#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <QObject>
#include <QMutex>
#include <QSharedPointer>
#include <stdint.h>
#include "ConfigRadio.h"
#include "ConfigLog.h"
#include "ConfigRadio171al.h"
#include "ConfigRadio171d.h"
#include "ConfigRadio181d.h"
#include "ConfigRadio781tcp.h"
#include "ConfigRadio212tcr.h"
#include "ConfigRadio220.h"
#include "ConfigRadio230.h"

class ConfigLoader
{
public:
    static ConfigLoader* getInstance();
    bool load();

    uint32_t getSysType()     const;
    uint16_t getProgramType() const;
    uint16_t getProgramID()   const;
    uint32_t getRadioTyp()    const;
    uint32_t getRadioID()     const;

    inline int getCtrlPort() const { return ctrlPort; }
    inline int getVoicPort() const { return voicPort; }

    inline QString getTcpIP()  const { return TcpIP; }
    inline int getTcpPort()    const { return TcpPort; }

    QSharedPointer<ConfigRadio> getConfigRadio();

private:
    ConfigLoader();
    ~ConfigLoader();

    void createConfig();
    void loadConfig();
    void loadRadioConfig();


private:
    static ConfigLoader     *m_instance;
    static QMutex           m_Mutex;

    uint32_t                sysType;
    uint16_t                programType;
    uint16_t                programID;
    uint32_t                radioTyp;
    uint32_t                radioID;

    int                     ctrlPort;
    int                     voicPort;

    //与通信平台通信配置信息
    QString                                 TcpIP;
    int                                     TcpPort;

    ConfigLog                               m_log;
    QSharedPointer<ConfigRadio>             m_radio171AL;
    QSharedPointer<ConfigRadio>             m_radio171D;
    QSharedPointer<ConfigRadio>             m_radio181D;
    QSharedPointer<ConfigRadio>             m_radio212TCR;
    QSharedPointer<ConfigRadio>             m_radio220;
    QSharedPointer<ConfigRadio>             m_radio781TCP;
    QSharedPointer<ConfigRadio>             m_radio230;
};

#endif // CONFIGLOADER_H

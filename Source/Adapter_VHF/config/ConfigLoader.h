#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <QObject>
#include <QMutex>

class ConfigLoader
{
public:
    static ConfigLoader* getInstance();
    bool load();


    QString getSndToIP() const;
    int getCtrlOnRevPort() const;
    int getCtrlSndToPort() const;
    int getVoicOnRevPort() const;
    int getVoicSndToPort() const;

    int getRadioID() const;
    QString getName() const;
    int getPosRevPort() const;
    QString getPosFromIP() const;
    int getPosFromPort() const;
    int getProgramType() const;
    int getProgramID() const;

    inline QString getCommunicateIP() const { return communicateIP; }
    inline int getDataTransPort() const { return dataTransPort; }
    inline int getCtlRevPort() const { return ctlRevPort; }
    inline int getCtlSndPort() const { return ctlSndPort; }

private:
    ConfigLoader();
    ~ConfigLoader();

    void createConfig();
    void loadConfig();


private:
    static ConfigLoader    *m_instance;
    static QMutex          m_Mutex;

    QString     sndToIP;
    int         ctrlOnRevPort;
    int         ctrlSndToPort;
    int         voicOnRevPort;
    int         voicSndToPort;
    int         radioID;
    QString     name;
    int         posRevPort;
    QString     posFromIP;
    int         posFromPort;
    int         programType;
    int         programID;
    //与通信平台通信配置信息
    QString     communicateIP;
    int         dataTransPort;
    int         ctlRevPort;
    int         ctlSndPort;

};

#endif // CONFIGLOADER_H

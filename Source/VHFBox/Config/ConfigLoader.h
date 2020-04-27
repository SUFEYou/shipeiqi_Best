#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H
#include <QSettings>
#include <QDebug>
#include <QMutex>
#include <QFileInfo>
#include "Config/BoxConf.h"


class ConfigLoader
{
public:
    static ConfigLoader* getInstance();
    bool load();


    int getProgramType() const;
    int getProgramID() const;

    int getPosRevPort() const;
    QString getPosFromIP() const;
    int getPosFromPort() const;


    inline BoxConf *getBoxConf1() const { return boxConf1; }
    inline BoxConf *getBoxConf2() const { return boxConf2; }
    inline BoxConf *getBoxConf3() const { return boxConf3; }
    inline BoxConf *getBoxConf4() const { return boxConf4; }


private:
    ConfigLoader();
    ~ConfigLoader();

    void createConfig();
    void loadConfig();


private:
    static ConfigLoader    *m_instance;
    static QMutex          m_Mutex;

//    QString     sndToIP;
//    int         ctrlOnRevPort;
//    int         ctrlSndToPort;
//    int         voicOnRevPort;
//    int         voicSndToPort;
//    int         radioID;
//    QString     name;

    int         programType;
    int         programID;

    int         posRevPort;
    QString     posFromIP;
    int         posFromPort;

    BoxConf     *boxConf1;
    BoxConf     *boxConf2;
    BoxConf     *boxConf3;
    BoxConf     *boxConf4;

};

#endif // CONFIGLOADER_H

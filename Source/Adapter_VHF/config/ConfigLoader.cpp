#include "ConfigLoader.h"
#include <QSettings>
#include <QDebug>
#include <QFileInfo>
#include <QFile>

ConfigLoader* ConfigLoader::m_instance = NULL;
QMutex ConfigLoader::m_Mutex;

ConfigLoader::ConfigLoader()
{

}

ConfigLoader::~ConfigLoader()
{

}

ConfigLoader* ConfigLoader::getInstance()
{

    QMutexLocker locker(&m_Mutex);
    if (m_instance == NULL)
    {
        m_instance = new ConfigLoader();
    }

    return m_instance;
}

bool ConfigLoader::load()
{
    if(QFile::exists("Config.ini")){
        qDebug() << "---------Loadin Config.ini---------";
        loadConfig();

    } else {

        qDebug() << "---------Create Config.ini---------";
        createConfig();
        loadConfig();
    }

    qDebug() << "-----------------------------------";

    return true;
}

void ConfigLoader::loadConfig()
{


    QSettings settings("Config.ini", QSettings::IniFormat);

    this->sndToIP        = settings.value("Setting/sndToIP").toString();
    this->ctrlOnRevPort  = settings.value("Radio-Ctrl/onRevPort").toInt();
    this->ctrlSndToPort  = settings.value("Radio-Ctrl/sndToPort").toInt();
    this->voicOnRevPort  = settings.value("Radio-Voice/onRevPort").toInt();
    this->voicSndToPort  = settings.value("Radio-Voice/sndToPort").toInt();

    this->radioID        = settings.value("Radio-Static/radioID").toInt();
    this->name           = settings.value("Radio-Static/name").toString();
    this->posRevPort     = settings.value("Position/posRevPort").toInt();
    this->posFromIP      = settings.value("Position/posFromIP").toString();
    this->posFromPort    = settings.value("Position/posFromPort").toInt();

    this->programType      = settings.value("ProgramInfo/ProgramType").toInt();
    this->programID    = settings.value("ProgramInfo/ProgramID").toInt();

    this->communicateIP = settings.value("Communicate/communicateIP").toString();
    this->dataTransPort = settings.value("Communicate/dataTransPort").toInt();
    this->ctlRevPort = settings.value("Communicate/ctlRevPort").toInt();
    this->ctlSndPort = settings.value("Communicate/ctlSndPort").toInt();

//    qDebug() << "SndTo-IP:" << sndToIP;
//    qDebug() << "Radio-Ctrl-onRevPort:"  << ctrlOnRevPort;
//    qDebug() << "Radio-Ctrl-sndToPort:"  << ctrlSndToPort;
//    qDebug() << "Radio-Voic-onRevPort:" << voicOnRevPort;
//    qDebug() << "Radio-Voic-sndToPort:" << voicSndToPort;
//    qDebug() << "Radio-static-radioID:" << radioID;
//    qDebug() << "Radio-static-name:"  << name;
//    qDebug() << "Position-posRevPort:"  << posRevPort;
//    qDebug() << "Position-posFromIP:" << posFromIP;
//    qDebug() << "Position-posFromPort:" << posFromPort;
//    qDebug() << "ProgramInfo-ProgramType:" << programType;
//    qDebug() << "ProgramInfo-ProgramID:" << programID;

    qDebug() << "Communicate IP: " << communicateIP;
    qDebug() << "Communicate dataTransPort: " << dataTransPort;
    qDebug() << "Communicate ctlRevPort: " << ctlRevPort;
    qDebug() << "Communicate ctlSndPort: " << ctlSndPort;
}





void ConfigLoader::createConfig()
{

    QSettings settings("Config.ini", QSettings::IniFormat);

    settings.beginGroup("ProgramInfo");
    settings.setValue("ProgramType",2152);
    settings.setValue("ProgramID",2152);
    settings.endGroup();

    settings.beginGroup("Radio-Static");
    settings.setValue("radioID",22152);
    settings.setValue("name", "VHFBox1");
    settings.endGroup();

    settings.beginGroup("Position");
    settings.setValue("posRevPort",60000);
    settings.setValue("posFromIP",   "192.168.1.15");
    settings.setValue("posFromPort",3030);
    settings.endGroup();

    settings.beginGroup("Setting");
    settings.setValue("sndToIP",   "192.168.1.15");
    settings.endGroup();

    settings.beginGroup("Radio-Ctrl");
    settings.setValue("onRevPort", 50001);
    settings.setValue("sndToPort", 50018);
    settings.endGroup();

    settings.beginGroup("Radio-Voice");
    settings.setValue("onRevPort", 50020);
    settings.setValue("sndToPort", 50017);
    settings.endGroup();

    settings.beginGroup("Communicate");
    settings.setValue("communicateIP",  "192.168.1.111");
    settings.setValue("dataTransPort", 12000);
    settings.setValue("ctlRevPort", 12201);
    settings.setValue("ctlSndPort", 12200);
    settings.endGroup();

}

int ConfigLoader::getVoicSndToPort() const
{
    return voicSndToPort;
}

int ConfigLoader::getVoicOnRevPort() const
{
    return voicOnRevPort;
}

int ConfigLoader::getCtrlSndToPort() const
{
    return ctrlSndToPort;
}

int ConfigLoader::getCtrlOnRevPort() const
{
    return ctrlOnRevPort;
}

QString ConfigLoader::getSndToIP() const
{
    return sndToIP;
}

QString ConfigLoader::getPosFromIP() const
{
    return posFromIP;
}

int ConfigLoader::getPosFromPort() const
{
    return posFromPort;
}

int ConfigLoader::getPosRevPort() const
{
    return posRevPort;
}

QString ConfigLoader::getName() const
{
    return name;
}

int ConfigLoader::getRadioID() const
{
    return radioID;
}

int ConfigLoader::getProgramID() const
{
    return programID;
}

int ConfigLoader::getProgramType() const
{
    return programType;
}

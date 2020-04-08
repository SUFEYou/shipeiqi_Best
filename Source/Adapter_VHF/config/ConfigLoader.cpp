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
    if(QFile::exists("Apt-Config.ini")){
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

    QSettings settings("Apt-Config.ini", QSettings::IniFormat);

    this->programType    = settings.value("Apt-Static/ProgramType").toInt();
    this->programID      = settings.value("Apt-Static/ProgramID").toInt();
    this->radioTyp       = settings.value("Apt-Static/RadioTyp").toInt();
    this->radioID        = settings.value("Apt-Static/RadioID").toInt();

    this->ctrlSndToIP    = settings.value("Radio-Ctrl/sndToIP").toString();
    this->ctrlOnRevPort  = settings.value("Radio-Ctrl/onRevPort").toInt();
    this->ctrlSndToPort  = settings.value("Radio-Ctrl/sndToPort").toInt();

    this->voicSndToIP    = settings.value("Radio-Voice/sndToIP").toString();
    this->voicOnRevPort  = settings.value("Radio-Voice/onRevPort").toInt();
    this->voicSndToPort  = settings.value("Radio-Voice/sndToPort").toInt();

    this->TcpIP          = settings.value("Commu-TCP/TcpIP").toString();
    this->TcpPort        = settings.value("Commu-TCP/TcpPort").toInt();


    qDebug() << "Commu Server TcpIP: "      << TcpIP;
    qDebug() << "Commu Server TcpPort: "    << TcpPort;

}


void ConfigLoader::createConfig()
{

    QSettings settings("Apt-Config.ini", QSettings::IniFormat);

    settings.beginGroup("Apt-Static");
    settings.setValue("PROGRAMTYPE", 13);
    settings.setValue("PROGRAMID",   6001);
    settings.setValue("RadioTyp", 1814);
    settings.setValue("RadioID",  10001);
    settings.endGroup();

    settings.beginGroup("Radio-Ctrl");
    settings.setValue("sndToIP",   "192.168.1.15");
    settings.setValue("onRevPort", 50001);
    settings.setValue("sndToPort", 50018);
    settings.endGroup();

    settings.beginGroup("Radio-Voice");
    settings.setValue("sndToIP",   "192.168.1.15");
    settings.setValue("onRevPort", 50020);
    settings.setValue("sndToPort", 50017);
    settings.endGroup();

    settings.beginGroup("Commu-TCP");
    settings.setValue("TcpIP",  "192.168.1.52");
    settings.setValue("TcpPort", 12000);
    settings.endGroup();


}


uint32_t ConfigLoader::getRadioID() const
{
    return radioID;
}

uint32_t ConfigLoader::getRadioTyp() const
{
    return radioTyp;
}

uint16_t ConfigLoader::getProgramID() const
{
    return programID;
}

uint16_t ConfigLoader::getProgramType() const
{
    return programType;
}





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

    this->programType = settings.value("Apt-Static/ProgramType").toInt();
    this->programID   = settings.value("Apt-Static/ProgramID").toInt();
    this->radioTyp    = settings.value("Apt-Static/RadioTyp").toInt();
    this->radioID     = settings.value("Apt-Static/RadioID").toInt();

    this->TcpIP     = settings.value("Commu-TCP/TcpIP").toString();
    this->TcpPort   = settings.value("Commu-TCP/TcpPort").toInt();

    this->ctrlPort  = settings.value("Radio-Ctrl/CtrlPort").toInt();
    this->voicPort  = settings.value("Radio-Voice/VoicPort").toInt();

    qDebug() << "Commu Server TcpIP: "      << TcpIP;
    qDebug() << "Commu Server TcpPort: "    << TcpPort;

}


void ConfigLoader::createConfig()
{

    QSettings settings("Apt-Config.ini", QSettings::IniFormat);

    settings.beginGroup("Apt-Static");
    settings.setValue("PROGRAMTYPE", 6);
    settings.setValue("PROGRAMID",   7998);
    settings.setValue("RadioTyp", 2204);
    settings.setValue("RadioID",  12998);
    settings.endGroup();

    settings.beginGroup("Commu-TCP");
    settings.setValue("TcpIP",  "192.168.1.52");
    settings.setValue("TcpPort", 12001);
    settings.endGroup();

    settings.beginGroup("Radio-Ctrl");
    settings.setValue("CtrlPort", 50018);
    settings.endGroup();

    settings.beginGroup("Radio-Voice");
    settings.setValue("VoicPort", 50017);
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





#include "ConfigLoader.h"
#include <QSettings>
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include "Radio/RadioType.h"

ConfigLoader* ConfigLoader::m_instance = NULL;
QMutex ConfigLoader::m_Mutex;

ConfigLoader::ConfigLoader()
             : m_radio171AL(new ConfigRadio171al)
             , m_radio171D(new ConfigRadio171d)
             , m_radio181D(new ConfigRadio181d)
             , m_radio212TCR(new ConfigRadio212tcr)
             , m_radio220(new ConfigRadio220)
             , m_radio781TCP(new ConfigRadio781tcp)
             , m_radio230(new ConfigRadio230)
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

//    qDebug() << "-----------------------------------";

    return true;
}

void ConfigLoader::loadConfig()
{

    QSettings settings("Apt-Config.ini", QSettings::IniFormat);

    this->TcpIP     = settings.value("Commu-TCP/TcpIP").toString();
    this->TcpPort   = settings.value("Commu-TCP/TcpPort").toInt();

    this->ctrlPort  = settings.value("Radio-Ctrl/CtrlPort").toInt();
    this->voicPort  = settings.value("Radio-Voice/VoicPort").toInt();

    qDebug() << "Commu Server TcpIP: "      << TcpIP;
    qDebug() << "Commu Server TcpPort: "    << TcpPort;

    this->sysType     = settings.value("Apt-Static/SYSTYPE").toInt();
    this->programType = settings.value("Apt-Static/ProgramType").toInt();
    this->programID   = settings.value("Apt-Static/ProgramID").toInt();
    this->radioTyp    = settings.value("Apt-Static/RadioTyp").toInt();
    this->radioID     = settings.value("Apt-Static/RadioID").toInt();
    loadRadioConfig();
}


void ConfigLoader::createConfig()
{

    QSettings settings("Apt-Config.ini", QSettings::IniFormat);

    settings.beginGroup("Apt-Static");
    settings.setValue("SYSTYPE", 0X0A01);
    settings.setValue("PROGRAMTYPE", 13);
    settings.setValue("PROGRAMID", 7999);
    settings.setValue("RadioTyp", RADIO_171D);
    settings.setValue("RadioID", 19999);
    settings.endGroup();

    settings.beginGroup("Commu-TCP");
    settings.setValue("TcpIP",  "192.168.1.15");
    settings.setValue("TcpPort", 12000);
    settings.endGroup();

    settings.beginGroup("Radio-Ctrl");
    settings.setValue("CtrlPort", 50018);
    settings.endGroup();

    settings.beginGroup("Radio-Voice");
    settings.setValue("VoicPort", 50017);
    settings.endGroup();

}

void ConfigLoader::loadRadioConfig()
{
    m_radio171AL->load();
    m_radio171D->load();
    m_radio181D->load();
    m_radio212TCR->load();
    m_radio220->load();
    m_radio781TCP->load();
    m_radio230->load();
}

QSharedPointer<ConfigRadio> ConfigLoader::getConfigRadio()
{
    if (radioTyp == RADIO_171AL)
        return m_radio171AL;
    else if (radioTyp == RADIO_171D)
        return m_radio171D;
    else if (radioTyp == RADIO_181D)
        return m_radio181D;
    else if (radioTyp == RADIO_212TCR)
        return m_radio212TCR;
    else if (radioTyp == RADIO_220)
        return m_radio220;
    else if (radioTyp == RADIO_781TCP)
        return m_radio781TCP;
    else if (radioTyp == RADIO_230)
        return m_radio230;
    else
        return QSharedPointer<ConfigRadio>(NULL);
}

uint32_t  ConfigLoader::getSysType()     const
{
    return sysType;
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




#include "ConfigRadio220.h"
#include <QSettings>
#include <QDebug>
#include <QFileInfo>
#include <QFile>

ConfigRadio220::ConfigRadio220()
{

}

ConfigRadio220::~ConfigRadio220()
{

}

void ConfigRadio220::load()
{
    if(QFile::exists("220-Config.ini"))
    {
        qDebug() << "---------Loadin 220 Config.ini---------";
        loadConfig();
    }
    else
    {
        qDebug() << "---------Create 220 Config.ini---------";
        createConfig();
        loadConfig();
    }
}

void ConfigRadio220::createConfig()
{
    QSettings settings("220-Config.ini", QSettings::IniFormat);

    settings.beginGroup("LinkInfo");
    settings.setValue("DATACOM", 10);
    settings.setValue("TIMERFACTOR", 40);
    settings.setValue("CIRCLEDRIFT", 10);
    settings.setValue("NOTINCHAINCTLMT", 300);
    settings.endGroup();
}

void ConfigRadio220::loadConfig()
{
    QSettings settings("220-Config.ini", QSettings::IniFormat);

    m_dataCom = settings.value("LinkInfo/DATACOM").toInt();
    m_timerFactor = settings.value("LinkInfo/TIMERFACTOR").toInt();
    m_circleDrift = settings.value("LinkInfo/CIRCLEDRIFT").toInt();
    m_notInChainCtLmt = settings.value("LinkInfo/NOTINCHAINCTLMT").toInt();
}

#include "ConfigRadio171al.h"
#include <QSettings>
#include <QDebug>
#include <QFileInfo>
#include <QFile>

ConfigRadio171al::ConfigRadio171al()
{

}

ConfigRadio171al::~ConfigRadio171al()
{

}

void ConfigRadio171al::load()
{
    if(QFile::exists("171al-Config.ini"))
    {
        qDebug() << "---------Loadin 171al Config.ini---------";
        loadConfig();
    }
    else
    {
        qDebug() << "---------Create 171al Config.ini---------";
        createConfig();
        loadConfig();
    }
}

void ConfigRadio171al::createConfig()
{
    QSettings settings("171al-Config.ini", QSettings::IniFormat);

    settings.beginGroup("LinkInfo");
    settings.setValue("TIMERFACTOR", 5);
    settings.setValue("CIRCLEDRIFT", 1);
    settings.setValue("NOTINCHAINCTLMT", 300);
    settings.endGroup();
}

void ConfigRadio171al::loadConfig()
{
    QSettings settings("171al-Config.ini", QSettings::IniFormat);

    m_timerFactor = settings.value("LinkInfo/TIMERFACTOR").toInt();
    m_circleDrift = settings.value("LinkInfo/CIRCLEDRIFT").toInt();
    m_notInChainCtLmt = settings.value("LinkInfo/NOTINCHAINCTLMT").toInt();
}

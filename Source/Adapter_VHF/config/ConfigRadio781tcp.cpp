#include "ConfigRadio781tcp.h"
#include <QSettings>
#include <QDebug>
#include <QFileInfo>
#include <QFile>

ConfigRadio781tcp::ConfigRadio781tcp()
{

}

ConfigRadio781tcp::~ConfigRadio781tcp()
{

}

void ConfigRadio781tcp::load()
{
    if(QFile::exists("781tcp-Config.ini"))
    {
        qDebug() << "---------Loadin 781tcp Config.ini---------";
        loadConfig();
    }
    else
    {
        qDebug() << "---------Create 781tcp Config.ini---------";
        createConfig();
        loadConfig();
    }
}

void ConfigRadio781tcp::createConfig()
{
    QSettings settings("781tcp-Config.ini", QSettings::IniFormat);

    settings.beginGroup("LinkInfo");
    settings.setValue("TIMERFACTOR", 5);
    settings.setValue("CIRCLEDRIFT", 1);
    settings.setValue("NOTINCHAINCTLMT", 300);
    settings.endGroup();
}

void ConfigRadio781tcp::loadConfig()
{
    QSettings settings("781tcp-Config.ini", QSettings::IniFormat);

    m_timerFactor = settings.value("LinkInfo/TIMERFACTOR").toInt();
    m_circleDrift = settings.value("LinkInfo/CIRCLEDRIFT").toInt();
    m_notInChainCtLmt = settings.value("LinkInfo/NOTINCHAINCTLMT").toInt();
}

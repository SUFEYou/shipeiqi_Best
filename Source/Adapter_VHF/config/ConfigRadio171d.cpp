#include "ConfigRadio171d.h"
#include <QSettings>
#include <QDebug>
#include <QFileInfo>
#include <QFile>

ConfigRadio171d::ConfigRadio171d()
{

}

ConfigRadio171d::~ConfigRadio171d()
{

}

void ConfigRadio171d::load()
{
    if(QFile::exists("171d-Config.ini"))
    {
        qDebug() << "---------Loadin 171d Config.ini---------";
        loadConfig();
    }
    else
    {
        qDebug() << "---------Create 171d Config.ini---------";
        createConfig();
        loadConfig();
    }
    qDebug() << "-----------------------------------";
}

void ConfigRadio171d::createConfig()
{
    QSettings settings("171d-Config.ini", QSettings::IniFormat);

    settings.beginGroup("LinkInfo");
    settings.setValue("TIMERFACTOR", 5);
    settings.setValue("CIRCLEDRIFT", 1);
    settings.setValue("NOTINCHAINCTLMT", 300);
    settings.endGroup();
}

void ConfigRadio171d::loadConfig()
{
    QSettings settings("171d-Config.ini", QSettings::IniFormat);

    m_timerFactor = settings.value("LinkInfo/TIMERFACTOR").toInt();
    m_circleDrift = settings.value("LinkInfo/CIRCLEDRIFT").toInt();
    m_notInChainCtLmt = settings.value("LinkInfo/NOTINCHAINCTLMT").toInt();
}

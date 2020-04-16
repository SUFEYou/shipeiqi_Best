#include "ConfigRadio181d.h"
#include <QSettings>
#include <QDebug>
#include <QFileInfo>
#include <QFile>

ConfigRadio181d::ConfigRadio181d()
{

}

ConfigRadio181d::~ConfigRadio181d()
{

}

void ConfigRadio181d::load()
{
    if(QFile::exists("181d-Config.ini"))
    {
        qDebug() << "---------Loadin 181d Config.ini---------";
        loadConfig();
    }
    else
    {
        qDebug() << "---------Create 181d Config.ini---------";
        createConfig();
        loadConfig();
    }
    qDebug() << "-----------------------------------";
}

void ConfigRadio181d::createConfig()
{
    QSettings settings("181d-Config.ini", QSettings::IniFormat);

    settings.beginGroup("LinkInfo");
    settings.setValue("TIMERFACTOR", 5);
    settings.setValue("CIRCLEDRIFT", 1);
    settings.setValue("NOTINCHAINCTLMT", 300);
    settings.endGroup();
}

void ConfigRadio181d::loadConfig()
{
    QSettings settings("181d-Config.ini", QSettings::IniFormat);

    m_timerFactor = settings.value("LinkInfo/TIMERFACTOR").toInt();
    m_circleDrift = settings.value("LinkInfo/CIRCLEDRIFT").toInt();
    m_notInChainCtLmt = settings.value("LinkInfo/NOTINCHAINCTLMT").toInt();
}

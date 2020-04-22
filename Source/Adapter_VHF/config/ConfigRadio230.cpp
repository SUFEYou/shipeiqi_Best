#include "ConfigRadio230.h"
#include <QSettings>
#include <QDebug>
#include <QFileInfo>
#include <QFile>

ConfigRadio230::ConfigRadio230()
{

}

ConfigRadio230::~ConfigRadio230()
{

}

void ConfigRadio230::load()
{
    if(QFile::exists("230-Config.ini"))
    {
        qDebug() << "---------Loadin 230 Config.ini---------";
        loadConfig();
    }
    else
    {
        qDebug() << "---------Create 230 Config.ini---------";
        createConfig();
        loadConfig();
    }
    qDebug() << "-----------------------------------";
}

void ConfigRadio230::createConfig()
{
    QSettings settings("230-Config.ini", QSettings::IniFormat);

    settings.beginGroup("LinkInfo");
    settings.setValue("TIMERFACTOR", 5);
    settings.setValue("CIRCLEDRIFT", 1);
    settings.setValue("NOTINCHAINCTLMT", 300);
    settings.endGroup();
}

void ConfigRadio230::loadConfig()
{
    QSettings settings("230-Config.ini", QSettings::IniFormat);

    m_timerFactor = settings.value("LinkInfo/TIMERFACTOR").toInt();
    m_circleDrift = settings.value("LinkInfo/CIRCLEDRIFT").toInt();
    m_notInChainCtLmt = settings.value("LinkInfo/NOTINCHAINCTLMT").toInt();
}

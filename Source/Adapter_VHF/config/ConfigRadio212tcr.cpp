#include "ConfigRadio212tcr.h"
#include <QSettings>
#include <QDebug>
#include <QFileInfo>
#include <QFile>

ConfigRadio212tcr::ConfigRadio212tcr()
{

}

ConfigRadio212tcr::~ConfigRadio212tcr()
{

}

void ConfigRadio212tcr::load()
{
    if(QFile::exists("212tcr-Config.ini"))
    {
        qDebug() << "---------Loadin 212tcr Config.ini---------";
        loadConfig();
    }
    else
    {
        qDebug() << "---------Create 212tcr Config.ini---------";
        createConfig();
        loadConfig();
    }
}

void ConfigRadio212tcr::createConfig()
{
    QSettings settings("212tcr-Config.ini", QSettings::IniFormat);

    settings.beginGroup("LinkInfo");
    settings.setValue("DATACOM", 10);
    settings.setValue("TIMERFACTOR", 40);
    settings.setValue("CIRCLEDRIFT", 10);
    settings.setValue("NOTINCHAINCTLMT", 300);
    settings.endGroup();
}

void ConfigRadio212tcr::loadConfig()
{
    QSettings settings("212tcr-Config.ini", QSettings::IniFormat);

    m_dataCom = settings.value("LinkInfo/DATACOM").toInt();
    m_timerFactor = settings.value("LinkInfo/TIMERFACTOR").toInt();
    m_circleDrift = settings.value("LinkInfo/CIRCLEDRIFT").toInt();
    m_notInChainCtLmt = settings.value("LinkInfo/NOTINCHAINCTLMT").toInt();
}

#include "ConfigLog.h"
#include <QSettings>
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include "log/log4z.h"

using namespace zsummer::log4z;

ConfigLog::ConfigLog()
{

}

ConfigLog::~ConfigLog()
{

}

void ConfigLog::load()
{
    if(QFile::exists("Log-Config.ini"))
    {
        qDebug() << "---------Loadin Log Config.ini---------";
        loadConfig();
    }
    else
    {
        qDebug() << "---------Create Log Config.ini---------";
        createConfig();
        loadConfig();
    }
}

void ConfigLog::createConfig()
{
    QSettings settings("Log-Config.ini", QSettings::IniFormat);

    settings.beginGroup("LogInfo");
    settings.setValue("LoggerPath", "./log");
    settings.setValue("LoggerDisplay", true);
    settings.setValue("LoggerFileLine", true);
    //0:TRACE 1:DEBUG 2:INFO 3:WARN 4:ERROR 5:ALARM 6:FATAL
    settings.setValue("LoggerLevel", 1);
    settings.endGroup();
}

void ConfigLog::loadConfig()
{
    QSettings settings("Log-Config.ini", QSettings::IniFormat);

    m_loggerPath = settings.value("LogInfo/LoggerPath").toString();
    m_loggerDisplay = settings.value("LogInfo/LoggerDisplay").toBool();
    m_loggerFileLine = settings.value("LogInfo/LoggerFileLine").toBool();
    //0:TRACE 1:DEBUG 2:INFO 3:WARN 4:ERROR 5:ALARM 6:FATAL
    m_loggerLevel = settings.value("LogInfo/LoggerLevel").toInt();

    ILog4zManager::getRef().setLoggerPath(LOG4Z_MAIN_LOGGER_ID, m_loggerPath.toStdString().c_str());
    ILog4zManager::getRef().setLoggerDisplay(LOG4Z_MAIN_LOGGER_ID, m_loggerDisplay);
    ILog4zManager::getRef().setLoggerFileLine(LOG4Z_MAIN_LOGGER_ID, m_loggerFileLine);
    ILog4zManager::getRef().setLoggerLevel(LOG4Z_MAIN_LOGGER_ID, m_loggerLevel);
    ILog4zManager::getRef().start();

    LOGI("Adapter Start");
}

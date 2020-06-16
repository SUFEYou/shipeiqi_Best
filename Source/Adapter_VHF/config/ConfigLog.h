#ifndef CONFIGLOG_H
#define CONFIGLOG_H
#include <QObject>

class ConfigLog
{
public:
    ConfigLog();
    ~ConfigLog();

    void load();

private:
    void createConfig();
    void loadConfig();

private:
    QString     m_loggerPath;
    bool        m_loggerDisplay;
    bool        m_loggerFileLine;
    quint8      m_loggerLevel;
    quint8      m_loggerLimitSize;
};

#endif // CONFIGLOG_H

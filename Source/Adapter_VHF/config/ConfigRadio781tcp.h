#ifndef CONFIGRADIO781TCP_H
#define CONFIGRADIO781TCP_H

#include "ConfigRadio.h"

class ConfigRadio781tcp : public ConfigRadio
{
public:
    ConfigRadio781tcp();
    virtual ~ConfigRadio781tcp();

    virtual void load();

protected:
    virtual void createConfig();
    virtual void loadConfig();
};

#endif // CONFIGRADIO781TCP_H

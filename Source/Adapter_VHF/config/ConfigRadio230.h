#ifndef CONFIGRADIO230_H
#define CONFIGRADIO230_H

#include "ConfigRadio.h"

class ConfigRadio230 : public ConfigRadio
{
public:
    ConfigRadio230();
    virtual ~ConfigRadio230();

    virtual void load();

protected:
    virtual void createConfig();
    virtual void loadConfig();
};

#endif // CONFIGRADIO230_H

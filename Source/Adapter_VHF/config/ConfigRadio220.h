#ifndef CONFIGRADIO220_H
#define CONFIGRADIO220_H

#include "ConfigRadio.h"

class ConfigRadio220 : public ConfigRadio
{
public:
    ConfigRadio220();
    virtual ~ConfigRadio220();

    virtual void load();

protected:
    virtual void createConfig();
    virtual void loadConfig();
};

#endif // CONFIGRADIO220_H

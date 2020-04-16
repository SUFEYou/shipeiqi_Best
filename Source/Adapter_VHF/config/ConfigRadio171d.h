#ifndef CONFIGRADIO171D_H
#define CONFIGRADIO171D_H

#include "ConfigRadio.h"

class ConfigRadio171d : public ConfigRadio
{
public:
    ConfigRadio171d();
    virtual ~ConfigRadio171d();

    virtual void load();

protected:
    virtual void createConfig();
    virtual void loadConfig();
};

#endif // CONFIGRADIO171D_H

#ifndef CONFIGRADIO181D_H
#define CONFIGRADIO181D_H

#include "ConfigRadio.h"

class ConfigRadio181d : public ConfigRadio
{
public:
    ConfigRadio181d();
    virtual ~ConfigRadio181d();

    virtual void load();

protected:
    virtual void createConfig();
    virtual void loadConfig();
};

#endif // CONFIGRADIO181D_H

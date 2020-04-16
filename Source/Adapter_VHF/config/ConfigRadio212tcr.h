#ifndef CONFIGRADIO212TCR_H
#define CONFIGRADIO212TCR_H

#include "ConfigRadio.h"

class ConfigRadio212tcr : public ConfigRadio
{
public:
    ConfigRadio212tcr();
    virtual ~ConfigRadio212tcr();

    virtual void load();

protected:
    virtual void createConfig();
    virtual void loadConfig();
};

#endif // CONFIGRADIO212TCR_H

#ifndef CONFIGRADIO171AL_H
#define CONFIGRADIO171AL_H

#include "ConfigRadio.h"

class ConfigRadio171al : public ConfigRadio
{
public:
    ConfigRadio171al();
    virtual ~ConfigRadio171al();

    virtual void load();

protected:
    virtual void createConfig();
    virtual void loadConfig();
};

#endif // CONFIGRADIO171AL_H

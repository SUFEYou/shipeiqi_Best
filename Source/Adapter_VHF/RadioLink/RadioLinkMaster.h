#ifndef RADIOLINKMASTER_H
#define RADIOLINKMASTER_H

#include "RadioLink/LinkCommon.h"
#include "RadioLink/RadioLink.h"

class RadioLinkMaster : public RadioLink
{
public:
    RadioLinkMaster();
    virtual ~RadioLinkMaster();

protected:
    virtual bool recvDataAnalyze(ObjMsg &msg);
};

#endif // RADIOLINKMASTER_H

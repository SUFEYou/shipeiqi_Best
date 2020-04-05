#ifndef RADIOLINKCLIENT_H
#define RADIOLINKCLIENT_H

#include "RadioLink/LinkCommon.h"
#include "RadioLink/RadioLink.h"

class RadioLinkClient : public RadioLink
{
public:
    RadioLinkClient();
    virtual ~RadioLinkClient();

protected:
    virtual bool recvDataAnalyze(ObjMsg &msg);
};

#endif // RADIOLINKCLIENT_H

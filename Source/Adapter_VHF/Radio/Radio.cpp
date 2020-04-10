#include "Radio.h"

Radio::Radio()
{

}

Radio::~Radio()
{
    if (ctrlCom != NULL)
    {
        delete ctrlCom;
        ctrlCom = NULL;
    }
    if (dataCom != NULL)
    {
        delete dataCom;
        dataCom = NULL;
    }
}

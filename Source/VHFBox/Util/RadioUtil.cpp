#include "RadioUtil.h"

RadioUtil::RadioUtil()
{

}

bool RadioUtil::isVHF(int radioTyp)
{

    if(radioTyp == 1814 || radioTyp == 1714
     ||radioTyp == 1716 || radioTyp == 7814)
    {
        return true;
    }

    return false;
}


QString RadioUtil::getRadioTypStr(int radioTyp)
{
    if(radioTyp == 1814){
        return "181D";
    }
    if(radioTyp == 1714){
        return "171D";
    }
    if(radioTyp == 1716){
        return "171AL";
    }
    if(radioTyp == 7814){
        return "781TCP";
    }
    if(radioTyp == 2124){
        return "212TCR";
    }

    return "";
}

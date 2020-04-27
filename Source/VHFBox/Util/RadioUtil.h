#ifndef RADIOUTIL_H
#define RADIOUTIL_H

#include <QString>

#pragma pack(1)
typedef enum Radio_Type	{
    RADIO_181D	= 1814,                 //
    RADIO_171D	= 1714,
    RADIO_171AL	= 1716,
    RADIO_781TCP= 7814,
    RADIO_212TCR= 2124,
    RADIO_220   = 2204,
    RADIO_230   = 2304,

}RadioTyp;


class RadioUtil
{
public:
    RadioUtil();

    static bool isVHF(int radioTyp);
    static QString getRadioTypStr(int radioTyp);
};

#endif // RADIOUTIL_H

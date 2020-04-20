#include "Radio.h"

Radio::Radio()
{

}

Radio::~Radio()
{

}

void Radio::bcd2uint8(uint8_t src, uint8_t* dst)
{
    *dst = (src>>4)*10 + (src&0x0F);
}

void Radio::uint82bcd(uint8_t src, uint8_t* dst)
{
    if (src > 99)
        *dst = 0;
    *dst = (src/10)<<4 | (src%10);
}

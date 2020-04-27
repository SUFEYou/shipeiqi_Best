#include "AudioUtil.h"

AudioUtil::AudioUtil()
{

}


/*****************************************************************************/
/* parametersArray2BitStream : convert bitStream to an array of parameters   */
/*             reverse operation of previous funtion                         */
/*    parameters:                                                            */
/*      -(i) bitStream : the 16 values streamed on 80 bits in a              */
/*           10*8bits values array                                           */
/*      -(o) parameters : 16 values parameters array                         */
/*                                                                           */
/*****************************************************************************/
void AudioUtil::parametersBitStream2Array(uint8_t bitStream[], uint16_t parameters[])
{
    parameters[0] = (bitStream[0]>>7)&(uint16_t)0x1;
    parameters[1] = bitStream[0]&(uint16_t)0x7f;
    parameters[2] = (bitStream[1]>>3)&(uint16_t)0x1f;
    parameters[3] = (((uint16_t)bitStream[1]&(uint16_t)0x7)<<2) | ((bitStream[2]>>6)&(uint16_t)0x3);
    parameters[4] = (((uint16_t)bitStream[2])&(uint16_t)0x3f)<<2 | ((bitStream[3]>>6)&(uint16_t)0x3);;
    parameters[5] = (bitStream[3]>>5)&(uint16_t)0x1;
    parameters[6] = (((uint16_t)(bitStream[3]&(uint16_t)0x1f))<<8)| bitStream[4];
    parameters[7] = (bitStream[5]>>4)&(uint16_t)0xf;
    parameters[8] = (bitStream[5]>>1)&(uint16_t)0x7;
    parameters[9] = (((uint16_t)bitStream[5]&(uint16_t)0x1)<<3)|((bitStream[6]>>5)&(uint16_t)0x7);
    parameters[10]= (uint16_t)bitStream[6]&(uint16_t)0x1f;
    parameters[11]= (((uint16_t)bitStream[7])<<5)|((bitStream[8]>>3)&(uint16_t)0x1f);
    parameters[12]= ((bitStream[8]&(uint16_t)0x7)<<1) | ((bitStream[9]>>7)&(uint16_t)0x1);
    parameters[13]= (bitStream[9]>>4)&(uint16_t)0x7;
    parameters[14]= bitStream[9]&(uint16_t)0xf;

    return;
}

/*** bitStream to parameters Array conversions functions ***/
/* Note: these functions are in utils.c because used by test source code too */

/*****************************************************************************/
/* parametersArray2BitStream : convert array of parameters to bitStream      */
/*      according to spec 4 - Table 8 and following mapping of values        */
/*               0 -> L0 (1 bit)                                             */
/*               1 -> L1 (7 bits)                                            */
/*               2 -> L2 (5 bits)                                            */
/*               3 -> L3 (5 bits)                                            */
/*               4 -> P1 (8 bit)                                             */
/*               5 -> P0 (1 bits)                                            */
/*               6 -> C1 (13 bits)                                           */
/*               7 -> S1 (4 bits)                                            */
/*               8 -> GA1(3 bits)                                            */
/*               9 -> GB1(4 bits)                                            */
/*               10 -> P2 (5 bits)                                           */
/*               11 -> C2 (13 bits)                                          */
/*               12 -> S2 (4 bits)                                           */
/*               13 -> GA2(3 bits)                                           */
/*               14 -> GB2(4 bits)                                           */
/*    parameters:                                                            */
/*      -(i) parameters : 16 values parameters array                         */
/*      -(o) bitStream : the 16 values streamed on 80 bits in a              */
/*           10*8bits values array                                           */
/*                                                                           */
/*****************************************************************************/
void AudioUtil::parametersArray2BitStream(uint16_t parameters[], uint8_t bitStream[])
{
    bitStream[0] = ((parameters[0]&((uint16_t) 0x1))<<7) |
            (parameters[1]&((uint16_t) 0x7f));

    bitStream[1] = ((parameters[2]&((uint16_t) 0x1f))<<3) |
            ((parameters[3]>>2)&((uint16_t) 0x7));

    bitStream[2] = ((parameters[3]&((uint16_t) 0x3))<<6) |
            ((parameters[4]>>2)&((uint16_t) 0x3f));

    bitStream[3] = ((parameters[4]&((uint16_t) 0x3))<<6) |
            ((parameters[5]&((uint16_t) 0x1))<<5) |
            ((parameters[6]>>8)&((uint16_t) 0x1f));

    bitStream[4] = ((parameters[6])&((uint16_t) 0xff));

    bitStream[5] = ((parameters[7]&((uint16_t) 0xf))<<4) |
            ((parameters[8]&((uint16_t) 0x7))<<1) |
            ((parameters[9]>>3)&((uint16_t) 0x1));

    bitStream[6] = ((parameters[9]&((uint16_t) 0x7))<<5) |
            (parameters[10]&((uint16_t) 0x1f));

    bitStream[7] = ((parameters[11]>>5)&((uint16_t) 0xff));

    bitStream[8] = ((parameters[11]&((uint16_t) 0x1f))<<3) |
            ((parameters[12]>>1)&((uint16_t) 0x7));

    bitStream[9] = ((parameters[12]&((uint16_t) 0x1))<<7) |
            ((parameters[13]&((uint16_t) 0x7))<<4) |
            (parameters[14]&((uint16_t) 0xf));

    return;
}

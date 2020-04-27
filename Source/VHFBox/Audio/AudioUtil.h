#ifndef AUDIOUTIL_H
#define AUDIOUTIL_H
#include "bcg729/encoder.h"
#include "bcg729/decoder.h"

#include "opus/opus.h"
#include "opus/opus_types.h"

class AudioUtil
{
public:
    AudioUtil();

    static void parametersBitStream2Array(uint8_t bitStream[], uint16_t parameters[]);
    static void parametersArray2BitStream(uint16_t parameters[], uint8_t bitStream[]);
};

#endif // AUDIOUTIL_H

#ifndef KSYNTH_WHITE_NOISE_H
#define KSYNTH_WHITE_NOISE_H


#include <stdexcept>

#include "daisysp.h"
#include "../AudioModule.h"


using namespace daisysp;


namespace kmodular
{
    
    class WhiteNoiseOsc: public AudioModule
    {
        public:
            WhiteNoise noise;

            WhiteNoiseOsc() {}
            ~WhiteNoiseOsc() {}
            void Init(float sampleRate);
            void Reset();
            void Process(const float* in, float* out, size_t sizeIn = 2, size_t sizeOut = 2);
            void Trigger(TriggerCommand command, int* intVals, float* floatVals);
    };

}


#endif

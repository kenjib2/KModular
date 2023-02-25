#ifndef KEFFECT_COSMOSIS_H
#define KEFFECT_COSMOSIS_H


#include "daisy_pod.h"
#include "daisysp.h"
#include "../AudioModule.h"


using namespace daisy;
using namespace daisysp;


namespace kmodular
{
    const int NUM_COSMOSIS_BUFFERS = 3;
    const int COSMOSIS_LFO_WAVEFORM = 0;
    const int MAX_COSMOSIS_DELAY = 8000;


    class Cosmosis: public AudioModule {
        public:
            float paramDryLevel = 1.f;
            float paramWetLevel = 1.f;
            int delayLengths[NUM_COSMOSIS_BUFFERS];  // Between 820 and 2050
            float time;
            float level;
            float speed;
            float depth;
            float doppler;

            Cosmosis() {}
            ~Cosmosis() {}
            void Init(float sampleRate);
            void Reset();
            void Process(const float* in, float* out, size_t sizeIn, size_t sizeOut);
            void Trigger(TriggerCommand command, int* intVals, float* floatVals);

        private:
    //        Delay* delayBuffer[NUM_COSMOSIS_BUFFERS];
    //        Modulation* delayModulation[NUM_COSMOSIS_BUFFERS];
    };
}


#endif

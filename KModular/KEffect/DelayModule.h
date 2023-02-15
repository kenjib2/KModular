#ifndef KEFFECT_DELAY_H
#define KEFFECT_DELAY_H


#include "daisy_pod.h"
#include "daisysp.h"
#include "../AudioModule.h"


using namespace daisy;
using namespace daisysp;


namespace kmodular
{

    // Set max delay time to 0.75 of samplerate.
    const int MAX_DELAY = (size_t)(96000 * 2.5f);
    static DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS delayL;
    static DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS delayR;


    class DelayModule: public AudioModule {
        private:
            int sampleRate;
            float currentDelay;

        public:
            float delayTime;
            float level;
            float feedback;

            DelayModule() {}
            ~DelayModule() {}
            void Init(float sampleRate);
            void Reset();
            void Process(const float* in, float* out, size_t sizeIn, size_t sizeOut);
            void Trigger(TriggerCommand command, int* intVals, float* floatVals);
    };

}


#endif

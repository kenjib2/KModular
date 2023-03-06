#ifndef KEFFECT_CHORUS_H
#define KEFFECT_CHORUS_H


#include "daisy_pod.h"
#include "daisysp.h"
#include "../AudioModule.h"


using namespace daisy;
using namespace daisysp;


namespace kmodular
{

    class ChorusModule: public AudioModule {
        public:
            Chorus chorusL;
            Chorus chorusR;
            float amount; // Useful range is 0.f - 0.5f

            ChorusModule() {}
            ~ChorusModule() {}
            void Init(float sampleRate);
            void Reset();
            void Process(const float* in, float* out, size_t sizeIn = 2, size_t sizeOut = 2);
            void Trigger(TriggerCommand command, int* intVals, float* floatVals);
    };
    
}


#endif

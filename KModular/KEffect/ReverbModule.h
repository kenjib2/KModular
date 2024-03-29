#ifndef KEFFECT_REVERB_H
#define KEFFECT_REVERB_H


#include "daisy_pod.h"
#include "daisysp.h"
#include "../AudioModule.h"


using namespace daisy;
using namespace daisysp;


namespace kmodular
{
    static ReverbSc DSY_SDRAM_BSS reverb; // Going into SDRAM adds 10% MCU utilization and uses 2% of total SDRAM

    class ReverbModule: public AudioModule {
        private:
            //ReverbSc reverb; // Uses about 76% of total available SRAM

        public:
            float level;
            float feedback;
            float lpFreq;

            ReverbModule() {}
            ~ReverbModule() {}
            void Init(float sampleRate);
            void Reset();
            void Process(const float* in, float* out, size_t sizeIn = 2, size_t sizeOut = 2);
            void Trigger(TriggerCommand command, int* intVals, float* floatVals);
    };
    
}


#endif

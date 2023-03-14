#ifndef KEFFECT_REPEATER_H
#define KEFFECT_REPEATER_H


#include "daisy_pod.h"
#include "daisysp.h"
#include "../AudioModule.h"
#include "delay/MultiTapDelay.h"


const int REPEATER_BUFFER_SECONDS = 2;


namespace kmodular
{

    class Repeater: public AudioModule {
        public:

            Repeater() {}
            ~Repeater() { if (delayEffectL != NULL) delete delayEffectL; if (delayEffectR != NULL) delete delayEffectR; }
            void Init(float sampleRate);
            void Reset();
            void Process(const float* in, float* out, size_t sizeIn = 2, size_t sizeOut = 2);
            void Trigger(TriggerCommand command, int* intVals, float* floatVals);

        private:
            int repeatMin;
            int repeatMax;
            int repeatLength;
            MultiTapDelay* delayEffectL = NULL;
            MultiTapDelay* delayEffectR = NULL;
    };

}


#endif

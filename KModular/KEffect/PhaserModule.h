#ifndef KEFFECT_PHASER_H
#define KEFFECT_PHASER_H


#include "daisy_pod.h"
#include "daisysp.h"
#include "../AudioModule.h"


using namespace daisy;
using namespace daisysp;


namespace kmodular
{

    static Phaser phaserL;
    static Phaser phaserR;

    class PhaserModule: public AudioModule {
        private:

        public:
            PhaserModule() {}
            ~PhaserModule() {}
            void Init(float sampleRate);
            void Reset();
            void Process(const float* in, float* out, size_t sizeIn, size_t sizeOut);
            void Trigger(TriggerCommand command, int* intVals, float* floatVals);
    };

}


#endif

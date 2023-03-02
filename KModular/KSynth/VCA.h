#ifndef KSYNTH_VCA_H
#define KSYNTH_VCA_H


#include <stdexcept>

#include "daisysp.h"
#include "../AudioModule.h"


using namespace daisysp;


namespace kmodular
{
    
    class VCA: public AudioModule
    {
        private:
            Adsr       env;
            Oscillator lfo;
            bool       noteTriggered;

        public:
            VCA() {}
            ~VCA() {}
            void Init(float sampleRate);
            void Reset();
            void Process(const float* in, float* out, size_t sizeIn = 2, size_t sizeOut = 2);
            void Trigger(TriggerCommand command, int* intVals, float* floatVals);
            bool IsPlaying();
            bool IsReleasing();
    };

}


#endif

#ifndef KSYNTH_VCF_H
#define KSYNTH_VCF_H


#include <stdexcept>

#include "daisysp.h"
#include "../AudioModule.h"


using namespace daisysp;


namespace kmodular
{
    
    class VCF: public AudioModule
    {
        private:
            bool            noteTriggered;
            float           frequency;
            float           resonance;
            float           envDepth;
            MoogLadder      filtL;
            MoogLadder      filtR;
            Adsr            env;
            Oscillator      lfo;
    
        public:
            VCF() {}
            ~VCF() {}
            void Init(float sampleRate);
            void Reset();
            void Process(const float* in, float* out, size_t sizeIn = 2, size_t sizeOut = 2);
            void Trigger(TriggerCommand command, int* intVals, float* floatVals);
    };

}


#endif

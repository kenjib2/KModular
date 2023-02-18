#ifndef KSYNTH_VCO_H
#define KSYNTH_VCO_H


#include <stdexcept>

#include "daisysp.h"
#include "../AudioModule.h"


using namespace daisysp;


namespace kmodular
{
    
    class VCO: public AudioModule
    {
        private:
            bool            noteTriggered;

            void CalculateBaseFreq();

        public:
            Oscillator osc;
            Adsr       env;
            Oscillator lfo;
            int        midiNote;
            float      baseFreq;
            float      envDepth;
            float      pitchOffset;
            float      globalPitchOffset;

            VCO() {}
            ~VCO() {}
            void Init(float sampleRate);
            void Reset();
            void Process(const float* in, float* out, size_t sizeIn = 2, size_t sizeOut = 2);
            void Trigger(TriggerCommand command, int* intVals, float* floatVals);
    };

}


#endif

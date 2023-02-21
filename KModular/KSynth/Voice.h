#ifndef KSYNTH_VOICE_H
#define KSYNTH_VOICE_H


#include <vector>
#include <stdexcept>

#include "daisysp.h"
#include "../AudioModule.h"
#include "VCO.h"
#include "WhiteNoiseOsc.h"
#include "VCF.h"
#include "VCA.h"


using namespace daisysp;


namespace kmodular
{
    
    const int DEFAULT_NUM_VCOS = 2;


    class Voice: public AudioModule
    {
        private:
            std::vector<VCO> vcos;
            WhiteNoiseOsc    whiteNoise;
            VCF              vcf;
            VCA              vca;
            float            level;

        public:
            bool             noteTriggered;
            int              currentMidiNote;
            Voice() {}
            ~Voice() {}
            void Init(float sampleRate);
            void Init(float sampleRate, int numVcos);
            void Reset();
            void Process(const float* in, float* out, size_t sizeIn = 2, size_t sizeOut = 2);
            void Trigger(TriggerCommand command, int* intVals, float* floatVals);
            bool IsAvailable();
    };

}


#endif

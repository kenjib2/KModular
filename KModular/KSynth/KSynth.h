#ifndef KSYNTH_H
#define KSYNTH_H


#include <stdexcept>
#include <vector>

#include "daisy_pod.h"
#include "daisysp.h"
#include "../AudioModule.h"
#include "Voice.h"
#include "../KEffect/ReverbModule.h"
#include "../KEffect/DelayModule.h"
#include "KSynthPatch.h"


using namespace daisy;
using namespace daisysp;
using namespace std;


namespace kmodular
{
    
    const int DEFAULT_NUM_VOICES = 8;
    const float MAX_ATTACK_TIME = 4.0f;
    const float MAX_DECAY_TIME = 4.0f;
    const float MAX_RELEASE_TIME = 4.0f;
    const float MAX_LFO_RATE = 4.0f;
    const float MAX_VCO_ENV_DEPTH = 12.0f;
    const float MAX_VCO_LFO_DEPTH = 12.0f;
    const float MIN_VCF_FREQUENCY = 10.0f;
    const float MAX_VCF_FREQUENCY = 18000.0f;


    class KSynth: public AudioModule
    {
        private:
            float           sampleRate;
            vector<Voice>   voices;
//            ReverbModule    reverb;
            DelayModule     delay;
            float           level;
            float           pitchOffset;
            int             ccSelectedVco;
            DaisyPod*       hw;

            Voice* RequestVoice(int midiNote);


        public:
            KSynth() {}
            ~KSynth() {}
            void Init(float sampleRate);
            void Init(float sampleRate, DaisyPod* hw, int numVoices);
            void Reset();
            void Process(const float* in, float* out, size_t sizeIn = 2, size_t sizeOut = 2);
            void Trigger(TriggerCommand command, int* intVals, float* floatVals);
            void LoadPatch(KSynthPatch* patch);
    };

}

#endif

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


using namespace daisy;
using namespace daisysp;
using namespace std;


namespace kmodular
{
    
    const int DEFAULT_NUM_VOICES = 8;


    class KSynth: AudioModule
    {
        private:
            vector<Voice>   voices;
            ReverbModule    reverb;
            DelayModule     delay;
            float           level;

            Voice* RequestVoice(int midiNote);


        public:
            KSynth() {}
            ~KSynth() {}
            void Init(float sampleRate);
            void Init(float sampleRate, int numVoices);
            void Reset();
            void Process(const float* in, float* out, size_t sizeIn = 2, size_t sizeOut = 2);
            void Trigger(TriggerCommand command, int* intVals, float* floatVals);
    };

}

#endif

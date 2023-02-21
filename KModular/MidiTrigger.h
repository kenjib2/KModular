#ifndef MIDITRIGGER_H
#define MIDITRIGGER_H


#include <vector>

#include "daisy_pod.h"
#include "AudioModule.h"


using namespace daisy;


namespace kmodular
{

    class MidiTrigger
    {
        private:
            DaisyPod*                   hw;
            std::vector<AudioModule*>    midiListeners;

        public:
            MidiTrigger() {}
            ~MidiTrigger() {}
            void Init(DaisyPod* hw);
            void AddMidiListener(AudioModule* midiListener);
            void Process();
            void HandleMidiMessage(MidiEvent* m);
    };

}


#endif

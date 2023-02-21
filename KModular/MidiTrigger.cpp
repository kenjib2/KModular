#include "MidiTrigger.h"


namespace kmodular
{

    void MidiTrigger::Init(DaisyPod* hw)
    {
        this->hw = hw;
        this->hw->midi.StartReceive();
    }
    

    void MidiTrigger::AddMidiListener(AudioModule* midiListener)
    {
        midiListeners.push_back(midiListener);
    }


    void MidiTrigger::Process()
    {
        hw->midi.Listen();
        while(hw->midi.HasEvents())
        {
            HandleMidiMessage(&(hw->midi.PopEvent()));
        }
    }


    void MidiTrigger::HandleMidiMessage(MidiEvent* m)
    {
        switch(m->type)
        {
            case NoteOn:
                break;

            case NoteOff:
                break;

            default:
                break;
        }
    }

}

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
            MidiEvent event = hw->midi.PopEvent();
            HandleMidiMessage(&event);
        }
    }


    void MidiTrigger::HandleMidiMessage(MidiEvent* m)
    {
        for (size_t i = 0; i < midiListeners.size(); i++) {
            if (midiListeners[i]->midiChannel == m->channel) {
                int intVals[2];
                NoteOnEvent on;
                NoteOffEvent off;
                ControlChangeEvent controlChange;
                switch(m->type)
                {
                    case NoteOn:
                        on = m->AsNoteOn();
                        intVals[0] = on.note;
                        intVals[1] = on.velocity;

                        // This is to avoid Max/MSP Note outs for now..
                        if(m->data[1] != 0)
                        {
                            midiListeners[i]->Trigger(NoteOn, intVals, NULL);
                        }
                        break;

                    case NoteOff:
                        off = m->AsNoteOff();
                        intVals[0] = off.note;
                        intVals[1] = off.velocity;

                        midiListeners[i]->Trigger(NoteOff, intVals, NULL);
                        break;

                    // Unimplemented message types
                    case ChannelPressure:
                    case PolyphonicKeyPressure:
                    case ProgramChange:
                    case PitchBend:
                    case SystemCommon:
                    case SystemRealTime:
                    case ChannelMode:
                    case MessageLast:
                        break;

                    case ControlChange:
                        controlChange = m->AsControlChange();
                        intVals[0] = controlChange.control_number;
                        intVals[1] = controlChange.value;
                        midiListeners[i]->Trigger(ParamChange, intVals, NULL);
                        break;

                    default:
                        break;
                }
            }
        }
    }

}

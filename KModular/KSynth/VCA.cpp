#include "VCA.h"


namespace kmodular
{
    
    void VCA::Init(float sampleRate)
    {
        noteTriggered = false;

        env.Init(sampleRate);
        lfo.Init(sampleRate);

        Reset();
    }


    void VCA::Reset()
    {
        env.SetAttackTime(0.002f);
        env.SetDecayTime(0.0f);
        env.SetSustainLevel(1.0f);
        env.SetReleaseTime(0.002f);

        lfo.SetWaveform(Oscillator::WAVE_TRI);
        lfo.SetFreq(1.0f);
        lfo.SetAmp(0.0f);
    }


    void VCA::Process(const float* in, float* out, size_t sizeIn, size_t sizeOut)
    {
        float envAmount = env.Process(noteTriggered);
        float lfoAmount = lfo.Process();
        for (size_t i = 0; i < sizeIn && i < sizeOut; i++) {
            out[i] = in[i] * envAmount * (1 - lfoAmount);
        }
    }


    void VCA::Trigger(TriggerCommand command, int* intVals, float* floatVals)
    {
        switch (command)
        {
            case TriggerNoteOn:
                noteTriggered = true;
                if (env.IsRunning()) {
                    env.Retrigger(false);
                } else {
                    lfo.Reset();
                }
                break;

            case TriggerNoteOff:
                noteTriggered = false;
                break;

            case TriggerParamChange:
                switch((SynthParam)intVals[0])
                {
                    case VcaEnvAttack:
                        env.SetAttackTime(floatVals[0]);
                        break;
                    case VcaEnvDecay:
                        env.SetDecayTime(floatVals[0]);
                        break;
                    case VcaEnvSustain:
                        env.SetSustainLevel(floatVals[0]);
                        break;
                    case VcaEnvRelease:
                        env.SetReleaseTime(floatVals[0]);
                        break;
 
                    case VcaLfoWaveform:
                        lfo.SetWaveform(intVals[1]);
                        break;
                    case VcaLfoRate:
                        lfo.SetFreq(floatVals[0]);
                        break;
                    case VcaLfoDepth:
                        lfo.SetAmp(floatVals[0]);
                        break;
                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }


    bool VCA::IsPlaying()
    {
        return env.IsRunning();
    }
    
}

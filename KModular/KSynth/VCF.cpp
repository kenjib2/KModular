#include "VCF.h"


namespace kmodular
{
    
    void VCF::Init(float sampleRate)
    {
        filter.Init(sampleRate);
        env.Init(sampleRate);
        lfo.Init(sampleRate);

        Reset();
    }


    void VCF::Reset()
    {
        noteTriggered = false;

        frequency = 18000.0f;
        resonance = 0.0f;
        filter.SetRes(resonance);
        filter.SetFreq(frequency);

        env.SetAttackTime(0.002f);
        env.SetDecayTime(0.0f);
        env.SetSustainLevel(1.0f);
        env.SetReleaseTime(10.0f);

        lfo.SetWaveform(Oscillator::WAVE_TRI);
        lfo.SetFreq(1.0f);
        lfo.SetAmp(0.0f);
    }


    void VCF::Process(const float* in, float* out, size_t sizeIn, size_t sizeOut)
    {
        if (sizeOut < 1 || sizeIn < 1) {
            return;
        }

        float envAmount = env.Process(noteTriggered) * envDepth;
        float lfoAmount = 1.0f - lfo.Process();
        float computedFrequency = frequency * lfoAmount + (18000 - frequency) * envAmount ;
        filter.SetFreq(computedFrequency);

        out[0] = filter.Process(in[0]);
    }


    void VCF::Trigger(TriggerCommand command, int* intVals, float* floatVals)
    {
        SynthParam synthParam;
        uint8_t waveform;

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
                synthParam = (SynthParam)intVals[0];
                waveform = Oscillator::WAVE_TRI;
                switch (synthParam)
                {
                    case VcfFrequency:
                        frequency = floatVals[0];
                        filter.SetFreq(frequency);
                        break;
                    case VcfResonance:
                        resonance = floatVals[0];
                        filter.SetRes(resonance);
                        break;

                    case VcfEnvAttack:
                        env.SetAttackTime(floatVals[0]);
                        break;
                    case VcfEnvDecay:
                        env.SetDecayTime(floatVals[0]);
                        break;
                    case VcfEnvSustain:
                        env.SetSustainLevel(floatVals[0]);
                        break;
                    case VcfEnvRelease:
                        env.SetReleaseTime(floatVals[0]);
                        break;
                    case VcfEnvDepth:
                        envDepth = floatVals[0];
                        break;

                    case VcfLfoWaveform:
                        waveform = (uint8_t)intVals[1];
                        lfo.SetWaveform(waveform);
                        break;
                    case VcfLfoRate:
                        lfo.SetFreq(floatVals[0]);
                        break;
                    case VcfLfoDepth:
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

}
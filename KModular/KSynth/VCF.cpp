#include "VCF.h"


namespace kmodular
{
    
    void VCF::Init(float sampleRate)
    {
        filtL.Init(sampleRate);
        filtR.Init(sampleRate);
        env.Init(sampleRate);
        lfo.Init(sampleRate);

        Reset();
    }


    void VCF::Reset()
    {
        noteTriggered = false;

        frequency = 18000.0f;
        resonance = 0.0f;
        filtL.SetRes(resonance);
        filtL.SetFreq(frequency);
        filtR.SetRes(resonance);
        filtR.SetFreq(frequency);

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
        if (sizeOut < 2) {
            return;
        }

        float envAmount = env.Process(noteTriggered) * envDepth + 1.0f - envDepth;
        float lfoAmount = 1.0f - lfo.Process();
        float computedFrequency = frequency * envAmount * lfoAmount;
        filtL.SetFreq(computedFrequency);
        filtR.SetFreq(computedFrequency);

        out[0] = filtL.Process(in[0]);
        out[1] = filtR.Process(in[1]);
    }


    void VCF::Trigger(TriggerCommand command, int* intVals, float* floatVals)
    {
        switch (command)
        {
            case NoteOn:
                noteTriggered = true;
                if (env.IsRunning()) {
                    env.Retrigger(false);
                } else {
                    lfo.Reset();
                }
                break;

            case NoteOff:
                noteTriggered = false;
                break;

            case ParamChange:
                SynthParam synthParam = (SynthParam)intVals[0];
                uint8_t waveform = Oscillator::WAVE_TRI;
                switch (synthParam)
                {
                    case VcfFrequency:
                        frequency = floatVals[0];
                        filtL.SetFreq(frequency);
                        filtR.SetFreq(frequency);
                        break;
                    case VcfResonance:
                        resonance = floatVals[0];
                        filtL.SetRes(resonance);
                        filtR.SetRes(resonance);
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
        }
    }

}
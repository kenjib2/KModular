#include "VCO.h"


namespace kmodular
{
    
    void VCO::Init(float sampleRate)
    {
        baseFreq = 100.0f;

        osc.Init(sampleRate);
        env.Init(sampleRate);
        lfo.Init(sampleRate);

        Reset();
    }


    void VCO::Reset()
    {
        noteTriggered = false;
        pitchOffset = 0.0f;
        globalPitchOffset = 0.0f;

        osc.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
        osc.SetFreq(baseFreq);
        osc.SetAmp(0.5f);

        env.SetAttackTime(0.0f);
        env.SetDecayTime(0.3f);
        env.SetSustainLevel(0.0f);
        env.SetReleaseTime(0.0f);
        envDepth = 0.0f;

        lfo.SetWaveform(Oscillator::WAVE_TRI);
        lfo.SetFreq(3.3f);
        lfo.SetAmp(0.03f);
    }


    void VCO::Process(const float* in, float* out, size_t sizeIn, size_t sizeOut)
    {
        if (sizeOut < 2) {
            return;
        }

        float envAmount = 1.0f + env.Process(noteTriggered) * envDepth;
        float lfoAmount = lfo.Process();
        float lfoModifier = (1.0f - lfoAmount / 2) + lfoAmount;
        float calculatedFreq = baseFreq * lfoModifier * envAmount;
        osc.SetFreq(calculatedFreq);

        float result = osc.Process();
        out[0] = result;
        out[1] = result;
    }


    void VCO::Trigger(TriggerCommand command, int* intVals, float* floatVals)
    {
        SynthParam synthParam;
        uint8_t waveform;

        switch (command)
        {
            case NoteOn:
                noteTriggered = true;
                midiNote = intVals[0];
                CalculateBaseFreq();
                
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
                synthParam = (SynthParam)intVals[0];
                waveform = Oscillator::WAVE_TRI;

                switch (synthParam)
                {
                    case VcoWaveform:
                        waveform = (uint8_t)intVals[2];
                        lfo.SetWaveform(waveform);
                        break;
                    case VcoLevel:
                        osc.SetAmp(floatVals[0]);
                        break;
                    case VcoPitchOffset:
                        pitchOffset = floatVals[0];
                        break;
                    case PitchOffset:
                        globalPitchOffset = floatVals[0];
                        break;

                    case VcoEnvAttack:
                        env.SetAttackTime(floatVals[0]);
                        break;
                    case VcoEnvDecay:
                        env.SetDecayTime(floatVals[0]);
                        break;
                    case VcoEnvSustain:
                        env.SetSustainLevel(floatVals[0]);
                        break;
                    case VcoEnvRelease:
                        env.SetReleaseTime(floatVals[0]);
                        break;
                    case VcoEnvDepth:
                        envDepth = floatVals[0];
                        break;

                    case VcoLfoWaveform:
                        waveform = (uint8_t)intVals[2];
                        lfo.SetWaveform(waveform);
                        break;
                    case VcoLfoRate:
                        lfo.SetFreq(floatVals[0]);
                        break;
                    case VcoLfoDepth:
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


    void VCO::CalculateBaseFreq()
    {
        baseFreq = mtof(midiNote + pitchOffset + globalPitchOffset);
    }

}

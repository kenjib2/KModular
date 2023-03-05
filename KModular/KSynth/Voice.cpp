#include "Voice.h"


namespace kmodular
{
    
    void Voice::Init(float sampleRate)
    {
        Init(sampleRate, DEFAULT_NUM_VCOS);
    }


    void Voice::Init(float sampleRate, int numVcos)
    {
        noteTriggered = false;
        currentMidiNote = 64;
        for (int i = 0; i < numVcos; i++) {
            VCO vco;
            vco.Init(sampleRate);
            vcos.push_back(vco);
        }
        whiteNoise.Init(sampleRate);
        vcf.Init(sampleRate);
        vca.Init(sampleRate);

        Reset();
    }


    void Voice::Reset()
    {
        level = 1.0f;
        for (size_t i = 0; i < vcos.size(); i++) {
            vcos[i].Reset();
        }
        whiteNoise.Reset();
        vcf.Reset();
        vca.Reset();
    }


    void Voice::Process(const float* in, float* out, size_t sizeIn, size_t sizeOut)
    {
        if (sizeOut < 2) {
            return;
        }

        float vcoOut = 0.0f;
        for (size_t i = 0; i < vcos.size(); i++) {
            float nextOut = 0.0f;
            vcos[i].Process(in, &nextOut, sizeIn, 1);
            vcoOut += nextOut;
        }

        float whiteNoiseOut = 0.0f;
        whiteNoise.Process(in, &whiteNoiseOut, sizeIn, 1);
        vcoOut += whiteNoiseOut;

        float vcfOut = 0.0f;
        vcf.Process(&vcoOut, &vcfOut, 1, 1);

        float vcaOut = 0.0f;
        vca.Process(&vcfOut, &vcaOut, 1, 1);

        out[0] = vcaOut * level;
        out[1] = vcaOut * level;
    }


    void Voice::Trigger(TriggerCommand command, int* intVals, float* floatVals)
    {
        SynthParam synthParam;

        switch (command)
        {
            case TriggerNoteOn:
                noteTriggered = true;
                currentMidiNote = intVals[0];

                for (size_t i = 0; i < vcos.size(); i++) {
                    vcos[i].Trigger(command, intVals, floatVals);
                }
                vcf.Trigger(command, intVals, floatVals);
                vca.Trigger(command, intVals, floatVals);

                break;

            case TriggerNoteOff:
                noteTriggered = false;

                for (size_t i = 0; i < vcos.size(); i++) {
                    vcos[i].Trigger(command, intVals, floatVals);
                }
                vcf.Trigger(command, intVals, floatVals);
                vca.Trigger(command, intVals, floatVals);

                break;

            case TriggerParamChange:
                synthParam = (SynthParam)intVals[0];
                int vcoNumber;

                // VCO Params
                if (synthParam >= VcoWaveform && synthParam <= VcoLfoDepth) {
                    vcoNumber = intVals[1];
                    if (vcoNumber == 0) {
                        // 0 means apply to all VCOs
                        for (size_t i = 0; i < vcos.size(); i++) {
                            vcos[i].Trigger(command, intVals, floatVals);
                        }
                        whiteNoise.Trigger(command, intVals, floatVals);
                    } else {
                        // Any other number indicates which VCO to trigger.
                        vcos[vcoNumber - 1].Trigger(command, intVals, floatVals);
                    }

                // Global pitch offset
                } else if (synthParam == PitchOffset) {
                    for (size_t i = 0; i < vcos.size(); i++) {
                        vcos[i].Trigger(command, intVals, floatVals);
                    }

                // Noise Params
                } else if (synthParam >= WhiteNoiseOscLevel && synthParam <= WhiteNoiseOscLevel) {
                    whiteNoise.Trigger(command, intVals, floatVals);

                // VCF Params
                } else if (synthParam >= VcfFrequency && synthParam <= VcfLfoDepth) {
                    vcf.Trigger(command, intVals, floatVals);

                // VCA Params
                } else if (synthParam >= VcaEnvAttack && synthParam <= VcaLfoDepth) {
                    vca.Trigger(command, intVals, floatVals);

                // Voice Level Params
                } else {
                    switch (synthParam)
                    {
                        case VoiceLevel:
                            level = floatVals[0];
                            break;

                        default:
                            break;

                    }
                }
                break;

            default:
                break;
        }
    }


    bool Voice::IsAvailable()
    {
        return !vca.IsPlaying();
    }


    bool Voice::IsReleasing()
    {
        return vca.IsReleasing();
    }

}

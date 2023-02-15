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
        vcf.Init(sampleRate);
        vca.Init(sampleRate);

        Reset();
    }


    void Voice::Reset()
    {
        for (size_t i = 0; i < vcos.size(); i++) {
            vcos[i].Reset();
        }
        vcf.Reset();
        vca.Reset();
    }


    void Voice::Process(const float* in, float* out, size_t sizeIn, size_t sizeOut)
    {
        if (sizeOut < 2) {
            return;
        }

        float vcoOut[2] = { 0.0f, 0.0f };
        for (size_t i = 0; i < vcos.size(); i++) {
            float nextOut[2] = { 0.0f, 0.0f };
            vcos[i].Process(in, nextOut, sizeIn, 2);
            vcoOut[0] += nextOut[0];
            vcoOut[1] += nextOut[1];
        }
        float vcfOut[2];
        vcf.Process(vcoOut, vcfOut, 2, 2);
        float vcaOut[2];
        vca.Process(vcfOut, vcaOut, 2, 2);

        out[0] = vcaOut[0];
        out[1] = vcaOut[1];
    }


    void Voice::Trigger(TriggerCommand command, int* intVals, float* floatVals)
    {
        switch (command)
        {
            case NoteOn:
                noteTriggered = true;
                currentMidiNote = intVals[0];
                break;

            case NoteOff:
                noteTriggered = false;
                break;

            case ParamChange:
                SynthParam synthParam = (SynthParam)intVals[0];

                // VCO Params
                if (synthParam >= VcoWaveform && synthParam <= VcoLfoDepth) {
                    for (size_t i = 0; i < vcos.size(); i++) {
                        vcos[i].Trigger(command, intVals, floatVals);
                    }

                // VCF Params
                } else if (synthParam >= VcfFrequency && synthParam <= VcfLfoDepth) {
                    vcf.Trigger(command, intVals, floatVals);

                // VCA Params
                } else if (synthParam >= VcaEnvAttack && synthParam <= VcaLfoDepth) {
                    vca.Trigger(command, intVals, floatVals);

                // Voice Level Params
                } else {
                }
                break;
        }

        for (size_t i = 0; i < vcos.size(); i++) {
            vcos[i].Trigger(command, intVals, floatVals);
        }
        vcf.Trigger(command, intVals, floatVals);
        vca.Trigger(command, intVals, floatVals);
    }


    bool Voice::IsAvailable()
    {
        return !vca.IsPlaying();
    }

}

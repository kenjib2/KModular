#include "PhaserModule.h"


namespace kmodular
{
    
    void PhaserModule::Init(float sampleRate)
    {
        phaserL.Init(sampleRate);
        phaserR.Init(sampleRate);

        Reset();
    }


    void PhaserModule::Reset()
    {
        phaserL.SetPoles(2);
        phaserR.SetPoles(2);

        phaserL.SetLfoDepth(0.3f);
        phaserR.SetLfoDepth(0.3f);

        phaserL.SetLfoFreq(0.3f);
        phaserR.SetLfoFreq(0.3f);

        phaserL.SetFreq(500.0f);
        phaserR.SetFreq(500.0f);

        phaserL.SetFeedback(0.2f);
        phaserR.SetFeedback(0.2f);
    }


    void PhaserModule::Process(const float* in, float* out, size_t sizeIn, size_t sizeOut)
    {
        if (sizeIn < 2 || sizeOut < 2) {
            return;
        }

        out[0] = phaserL.Process(in[0] * 0.5f);
        out[1] = phaserR.Process(in[1] * 0.5f);
    }


    void PhaserModule::Trigger(TriggerCommand command, int* intVals, float* floatVals)
    {
        switch (command)
        {
            case ParamChange:
                switch((SynthParam)intVals[0])
                {
                    case PhaserNumPoles:
                        phaserL.SetPoles(intVals[1]);
                        phaserR.SetPoles(intVals[1]);
                        break;
                    case PhaserLfoDepth:
                        phaserL.SetLfoDepth(floatVals[0]);
                        phaserR.SetLfoDepth(floatVals[0]);
                        break;
                    case PhaserFrequency:
                        phaserL.SetLfoFreq(floatVals[0]);
                        phaserR.SetLfoFreq(floatVals[0]);
                        break;
                    case PhaserAllPassFrequency:
                        phaserL.SetFreq(floatVals[0]);
                        phaserR.SetFreq(floatVals[0]);
                    case PhaserFeedback:
                        phaserL.SetFeedback(floatVals[0]);
                        phaserR.SetFeedback(floatVals[0]);

                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

}
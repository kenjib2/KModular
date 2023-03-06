#include "ChorusModule.h"


namespace kmodular
{
    
    void ChorusModule::Init(float sampleRate)
    {
        chorusL.Init(sampleRate);
        chorusR.Init(sampleRate);

        Reset();
    }


    void ChorusModule::Reset()
    {
        amount = 1.0f;
        chorusL.SetDelay(.75);
        chorusR.SetDelay(.75);
        chorusL.SetLfoFreq(.3f);
        chorusR.SetLfoFreq(.3f);
        chorusL.SetLfoDepth(.9f);
        chorusR.SetLfoDepth(.9f);
        chorusL.SetFeedback(.2f);
        chorusR.SetFeedback(.2f);
    }


    void ChorusModule::Process(const float* in, float* out, size_t sizeIn, size_t sizeOut)
    {
        if (sizeIn < 2 || sizeOut < 2) {
            return;
        }
        out[0] = chorusL.Process(in[0]) * amount + in[0] * (1.f - amount);
        out[1] = chorusR.Process(in[1]) * amount + in[1] * (1.f - amount);
    }


    void ChorusModule::Trigger(TriggerCommand command, int* intVals, float* floatVals)
    {
        switch (command)
        {
            case TriggerParamChange:
                switch((SynthParam)intVals[0])
                {
                    case ChorusAmount:
                        amount = floatVals[0];
                        break;
                    case ChorusDelay:
                        chorusL.SetDelay(floatVals[0]);
                        chorusR.SetDelay(floatVals[0]);
                        break;
                    case ChorusFeedback:
                        chorusL.SetFeedback(floatVals[0]);
                        chorusR.SetFeedback(floatVals[0]);
                        break;
                    case ChorusLfoFreq:
                        chorusL.SetLfoFreq(floatVals[0]);
                        chorusR.SetLfoFreq(floatVals[0]);
                        break;
                    case ChorusLfoDepth:
                        chorusL.SetLfoDepth(floatVals[0]);
                        chorusR.SetLfoDepth(floatVals[0]);
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

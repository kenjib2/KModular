#include "ReverbModule.h"


namespace kmodular
{
    
    void ReverbModule::Init(float sampleRate)
    {
        reverb.Init(sampleRate);

        Reset();
    }


    void ReverbModule::Reset()
    {
        level = 0.10f;
        feedback = 0.50f;
        reverb.SetFeedback(feedback);
        lpFreq = 12000.0f;
        reverb.SetLpFreq(lpFreq);
    }


    void ReverbModule::Process(const float* in, float* out, size_t sizeIn, size_t sizeOut)
    {
        float sendL = in[0] * level;
        float sendR = in[1] * level;
        float wetL = 0.0f;
        float wetR = 0.0f;

        reverb.Process(sendL, sendR, &wetL, &wetR);
        out[0] = (in[0] + wetL);
        out[1] = (in[1] + wetR);
    }


    void ReverbModule::Trigger(TriggerCommand command, int* intVals, float* floatVals)
    {
        switch (command)
        {
            case ParamChange:
                switch((SynthParam)intVals[0])
                {
                    case ReverbLevel:
                        level = floatVals[0];
                        break;
                    case ReverbFeedback:
                        reverb.SetFeedback(floatVals[0]);
                        break;
                    case ReverbLpFreq:
                        reverb.SetLpFreq(floatVals[0]);
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

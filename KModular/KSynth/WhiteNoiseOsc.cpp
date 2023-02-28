#include "WhiteNoiseOsc.h"


namespace kmodular
{
    
    void WhiteNoiseOsc::Init(float sampleRate)
    {
        noise.Init();
        Reset();
    }


    void WhiteNoiseOsc::Reset()
    {
        noise.SetAmp(0.0f);
    }


    void WhiteNoiseOsc::Process(const float* in, float* out, size_t sizeIn, size_t sizeOut)
    {
        if (sizeOut < 2) {
            return;
        }

        float result = noise.Process();
        out[0] = result;
        out[1] = result;
    }


    void WhiteNoiseOsc::Trigger(TriggerCommand command, int* intVals, float* floatVals)
    {
        switch (command)
        {
            SynthParam synthParam;
            case TriggerParamChange:
                synthParam = (SynthParam)intVals[0];
                switch (synthParam)
                {
                    case WhiteNoiseOscLevel:
                        noise.SetAmp(floatVals[0]);
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

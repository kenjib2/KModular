#include "DelayModule.h"


namespace kmodular
{

    void DelayModule::Init(float sampleRate)
    {
        this->sampleRate = sampleRate;
        delayL.Init();
        delayR.Init();
        Reset();
    }


    void DelayModule::Reset()
    {
        delayTime = 0.75f;
        delayL.SetDelay(delayTime);
        delayR.SetDelay(delayTime);

        level = 0.5f;
        feedback = 0.3f;
    }


    void DelayModule::Process(const float* in, float* out, size_t sizeIn = 2, size_t sizeOut = 2)
    {
        fonepole(currentDelay, delayTime, .00007f);
        delayL.SetDelay(currentDelay);
        delayR.SetDelay(currentDelay);

        float outL = delayL.Read();
        float outR = delayR.Read();

        delayL.Write((feedback * outL) + in[0]);
        out[0] = (level * outL) + in[0];

        delayR.Write((feedback * outR) + in[0]);
        out[1] = (level * outR) + in[1];
    }


    void DelayModule::Trigger(TriggerCommand command, int* intVals, float* floatVals)
    {
        switch (command)
        {
            case ParamChange:
                switch((SynthParam)intVals[0])
                {
                    case DelayTime:
                        delayTime = floatVals[0];
                        break;
                    case DelayLevel:
                        level = floatVals[0];
                        break;
                    case DelayFeedback:
                        feedback = floatVals[0];
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

#include "Repeater.h"


namespace kmodular
{

    void Repeater::Init(float sampleRate)
    {
        int bufferSize = REPEATER_BUFFER_SECONDS * (int)(sampleRate + __FLT_EPSILON__);
    	delayEffectL = new MultiTapDelay(bufferSize);
//    	delayEffectR = new MultiTapDelay(bufferSize);
    	delayEffectL->setNumTaps(15);
//    	delayEffectR->setNumTaps(15);
        repeatMin = 1000;
        repeatMax = bufferSize/16;

        Reset();
    }


    void Repeater::Reset()
    {
        repeatLength = 4000;
        for (int i = 0; i < 15; i++) {
            delayEffectL->setTapDelayLength(i, repeatLength * (i + 1));
//            delayEffectR->setTapDelayLength(i, repeatLength * (i + 1));
        }
        delayEffectL->setPrimaryDelayLevel(0.f);
//        delayEffectR->setPrimaryDelayLevel(0.f);
        delayEffectL->setTapDelayLevel(0, 0.2f);
//        delayEffectR->setTapDelayLevel(0, 0.2f);
        delayEffectL->setTapDelayLevel(1, 0.0f);
//        delayEffectR->setTapDelayLevel(1, 0.0f);
        delayEffectL->setTapDelayLevel(2, 0.8f);
//        delayEffectR->setTapDelayLevel(2, 0.8f);
        delayEffectL->setTapDelayLevel(3, 0.0f);
//        delayEffectR->setTapDelayLevel(3, 0.0f);
        delayEffectL->setTapDelayLevel(4, 0.2f);
//        delayEffectR->setTapDelayLevel(4, 0.2f);
        delayEffectL->setTapDelayLevel(5, 0.0f);
//        delayEffectR->setTapDelayLevel(5, 0.0f);
        delayEffectL->setTapDelayLevel(6, 0.8f);
//        delayEffectR->setTapDelayLevel(6, 0.8f);
        delayEffectL->setTapDelayLevel(7, 0.0f);
//        delayEffectR->setTapDelayLevel(7, 0.0f);
        delayEffectL->setTapDelayLevel(8, 0.2f);
//        delayEffectR->setTapDelayLevel(8, 0.2f);
        delayEffectL->setTapDelayLevel(9, 0.3f);
//        delayEffectR->setTapDelayLevel(9, 0.3f);
        delayEffectL->setTapDelayLevel(10, 0.5f);
//        delayEffectR->setTapDelayLevel(10, 0.5f);
        delayEffectL->setTapDelayLevel(11, 0.6f);
//        delayEffectR->setTapDelayLevel(11, 0.6f);
        delayEffectL->setTapDelayLevel(12, 0.7f);
//        delayEffectR->setTapDelayLevel(12, 0.7f);
        delayEffectL->setTapDelayLevel(13, 0.8f);
//        delayEffectR->setTapDelayLevel(13, 0.8f);
        delayEffectL->setTapDelayLevel(14, 0.9f);
//        delayEffectR->setTapDelayLevel(14, 0.9f);
        delayEffectL->paramReverse = false;
//        delayEffectR->paramReverse = false;
        delayEffectL->paramDry = 1.f;
//        delayEffectR->paramDry = 1.f;
        delayEffectL->paramWet = 1.f;
//        delayEffectR->paramWet = 1.f;
        delayEffectL->paramFeedback = 0.f;
//        delayEffectR->paramFeedback = 0.f;
    }


    void Repeater::Process(const float* in, float* out, size_t sizeIn, size_t sizeOut)
    {
		float nextSample = in[0];
		nextSample = delayEffectL->processSample(nextSample);
		out[0] = nextSample;
		out[1] = nextSample;

//        out[0] = in[0];
//        out[1] = in[1];
    }


    void Repeater::Trigger(TriggerCommand command, int* intVals, float* floatVals)
    {
        switch (command)
        {
            case TriggerParamChange:
                switch((SynthParam)intVals[0])
                {
/*                    case ReverbLevel:
                        level = floatVals[0];
                        break;
                    case ReverbFeedback:
                        reverb.SetFeedback(floatVals[0]);
                        break;
                    case ReverbLpFreq:
                        reverb.SetLpFreq(floatVals[0]);
                        break;*/

                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

}

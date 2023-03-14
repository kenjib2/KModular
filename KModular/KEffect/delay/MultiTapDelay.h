#ifndef KEFFECT_MULTI_TAP_DELAY
#define KEFFECT_MULTI_TAP_DELAY


#include "CircularBuffer.h"


namespace kmodular
{

  class MultiTapDelay {
    public:
      bool paramReverse = false;
      float paramDry = 0.8;
      float paramWet = 0.5;
      float paramFeedback = 0.0;

      MultiTapDelay(int bufferSize = MAX_DELAY_BUFFER_SIZE);
      ~MultiTapDelay();

      float processSample(float inputSample);
      void setPrimaryDelayLength(int delaySamples);
      void setPrimaryDelayLevel(float level);
      void setTapDelayLength(int indexNumber, int delaySamples);
      void setTapDelayLevel(int indexNumber, float level);
      void setNumTaps(int numTaps);
      void setWriteInsert(WriteInsert* writeInsertObject);

    protected:
      AudioBuffer* primaryDelayBuffer;
      AudioBuffer* tapDelayBuffer;
  };

}


#endif

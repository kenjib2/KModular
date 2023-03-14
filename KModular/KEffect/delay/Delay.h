#ifndef KEFFECT_DELAY_H
#define KEFFECT_DELAY_H


#include "daisy_pod.h"
#include "CircularBuffer.h"


namespace kmodular
{

  #define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))


  class Delay {
    public:
      bool paramReverse = false;
      float paramDry = 0.8; // Between 0.0 and 1.0
      float paramWet = 0.2; // Between 0.0 and 1.0
      float paramFeedback = 0.2; // Between 0.0 and 1.0
      int paramSensitivity = 32767; // Threshold for delay buffer writes in samples between 0 (always off) and 32767 (always on)
      int paramTimeInSamples = MIN_DELAY_TIME_PARAM;

      Delay(int bufferSize = MAX_DELAY_BUFFER_SIZE);
      ~Delay();
      
      float processSample(float inputSample);
      void setDelayLength(int delaySamples);
      void setLoopLatch(float loopLatch); // Between 0.0 and 1.0
      void setDrift(float drift); // Between 0.0 and 1.0
      void setTimeLatch(float timeLatch); // Between 0.0 and 1.0
      void setWriteInsert(WriteInsert* writeInsertObject);

    protected:
      float paramLoopLatch = 0.0; // Between 0.0 and 1.0
      float paramLoopLatchLog = paramLoopLatch * paramLoopLatch; // We want a logorithmic response rate
      float paramDrift = 0.f; // Between 0.0 and 1.0
      float paramDriftLog = paramDrift * paramDrift; // Between 0.0 and 1.0
      float paramTimeLatch = 0.f; // Between 0.0 and 1.0
      float paramTimeLatchLog = paramTimeLatch * paramTimeLatch; // Between 0.0 and 1.0
      AudioBuffer * delayBuffer;
      int delaySize = 0;
      int loopPosition = 0;
      int fadePosition = 0;
      bool sensitivityTriggered;
      int peakReleaseCount = 0;
      bool isLoopLatched = false;
      int loopLatchCount = 0;
      int nextDelayLength = 0;
      int timeLatchCount = 0;
      int driftOffset = 0;

      void processLoopStart();
  };

}


#endif

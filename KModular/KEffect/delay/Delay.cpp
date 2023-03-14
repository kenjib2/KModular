#include "Delay.h"


namespace kmodular
{

  Delay::Delay(int bufferSize) {
      delayBuffer = new CircularBuffer(bufferSize);
      delayBuffer->numReadIndices = 1;
      delayBuffer->setDelayLevel(0, 1.0f);
      sensitivityTriggered = false;
  }


  Delay::~Delay() {
    delete delayBuffer;
  }


  void Delay::setDelayLength(int delaySamples) {
      // We won't make the actual change until the start of the next loop
      nextDelayLength = delaySamples;
  }


  void Delay::setLoopLatch(float loopLatch) {
      paramLoopLatch = loopLatch;
      paramLoopLatchLog = paramLoopLatch * paramLoopLatch * paramLoopLatch; // We want a logorithmic response rate
  }


  void Delay::setDrift(float drift) {
      paramDrift = drift;
      paramDriftLog = paramDrift * paramDrift * paramDrift; // We want a logorithmic response rate
  }


  void Delay::setTimeLatch(float timeLatch) {
      paramTimeLatch = timeLatch;
      paramTimeLatchLog = paramTimeLatch * paramTimeLatch * paramTimeLatch; // We want a logorithmic response rate
  }


  void Delay::setWriteInsert(WriteInsert* writeInsertObject) {
      delayBuffer->setWriteInsert(writeInsertObject);
  }


  void Delay::processLoopStart() {
    // Managing latches this cycle -- don't latch if we aren't triggered
    if (!isLoopLatched && sensitivityTriggered) {
      float rand = daisy::Random::GetFloat();
      if (rand > (1.f - paramLoopLatchLog)) {
        isLoopLatched = true;
        loopLatchCount = 15 - 8 * rand;
        delayBuffer->startLatch(0);
  //      Serial.println("Latch on");
      }
    }
    if (isLoopLatched && loopLatchCount <= 0) {
      isLoopLatched = false;
      delayBuffer->stopLatch();
  //    Serial.println("Latch off");
    }
    if (loopLatchCount > 0) {
      loopLatchCount--;
    }

    // Setting the offsets for drift
    // Don't drift or apply size changes when we are loop latched or time latched
    if (!isLoopLatched && timeLatchCount == 0) {
        float offsetWeighting = daisy::Random::GetFloat() * paramDriftLog - paramDriftLog / 2;
        if (offsetWeighting < 0) {
            driftOffset = int((delayBuffer->getDelaySize(0) - MIN_DELAY_TIME_PARAM * 44100) * offsetWeighting);
        }
        else {
            driftOffset = int((paramTimeInSamples * 4) * offsetWeighting);
        }

        // Apply any delay size changes only at the start of a new loop
        paramTimeInSamples = nextDelayLength;
        int driftedDelayLength = constrain(paramTimeInSamples + driftOffset, int(MIN_DELAY_TIME_PARAM * 44100), MAX_DELAY_BUFFER_SIZE); // We have to calculate loopsize here to include drift
        delayBuffer->setDelaySize(0, driftedDelayLength);
    }

  }


  float Delay::processSample(float inputSample) {
      if (abs(inputSample) > 32767 - paramSensitivity) {
          peakReleaseCount = 0;
          sensitivityTriggered = true;  // Sensitivity will turn off latch when the threshold is exceeded
      }
      else if (sensitivityTriggered) {
          peakReleaseCount++;
          if (peakReleaseCount > delaySize) {
              sensitivityTriggered = false;
          }
      }

      // processLoopStart checks for whether sensitivity is triggered, so it must run after that check.
      if (loopPosition == 0) {
          processLoopStart();
      }

      float readSample = delayBuffer->calculateReadSample(inputSample, paramReverse, paramDry, paramWet);
      float writeSample = 0;

    // Sensitivity fade in and out to prevent pops.
    float calculatedInputSample = inputSample;
    if (sensitivityTriggered) {
      // Fade in when sensitivity is triggered
      if (fadePosition > 0) {
        calculatedInputSample = (float)calculatedInputSample * (FADE_SAMPLES - fadePosition) / FADE_SAMPLES;
        fadePosition--;
      }
    }
    else {
      // Fade out when sensitivity ends
      if (fadePosition < FADE_SAMPLES) {
        calculatedInputSample = (float)calculatedInputSample * (FADE_SAMPLES - fadePosition) / FADE_SAMPLES;
        fadePosition++;
      }
      else {
        calculatedInputSample = 0;
      }
    }

    writeSample = delayBuffer->calculateWriteSample(calculatedInputSample, paramReverse, paramFeedback);
    delayBuffer->writeNextSample(writeSample);
    delayBuffer->next(paramReverse);
    loopPosition++;
    if (loopPosition >= delayBuffer->getDelaySize(0)) {
        loopPosition = 0;
    }

    return readSample;
  }

}

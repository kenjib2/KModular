#ifndef KEFFECT_AUDIOBUFFER_H
#define KEFFECT_AUDIOBUFFER_H


#include "../../BufferManager.h"


namespace kmodular
{

  const float MIN_DELAY_TIME_PARAM = 0.020f;
  const float MAX_DELAY_BUFFER_SECONDS = 1.0;
  const int DELAY_SAMPLE_RATE = 48000;
  const int MAX_DELAY_BUFFER_SIZE = (int)(DELAY_SAMPLE_RATE * MAX_DELAY_BUFFER_SECONDS);
  const int MAX_READ_INDICES = 16;
  const int FADE_SAMPLES = 100;


  class WriteInsert {
  public:
    virtual ~WriteInsert() = default;

    virtual float processSample(float sampleIn) = 0;
  };


  class AudioBuffer {

  protected:
    int writeIndex;
    int readIndices[MAX_READ_INDICES];
    int delaySizes[MAX_READ_INDICES];
    float delayLevels[MAX_READ_INDICES];
    WriteInsert* writeInsert = 0;
    bool isLatched = false;
    bool isFirstLatch = false;
    int latchSize;

    float crossFade(float sampleA, float aCoefficient, float sampleB);

  public:
    float* audioBuffer;
    int bufferSize;
    int numReadIndices;
    int prevReadIndices[MAX_READ_INDICES];

    AudioBuffer(int bufferSize);
    virtual ~AudioBuffer();

    void setDelaySize(int indexNumber, int numSamples); // If you change delay size more than once within FADE_BUFFER samples, there will be audible pops
    int getDelaySize(int indexNumber);
    void setDelayLevel(int indexNumber, float level); // Level between 0.0f and 1.0f
    int getReadIndex(int indexNumber);
    void setWriteInsert(WriteInsert* writeInsertObject);
    void startLatch(int indexNumber);
    void stopLatch();
    virtual void next(bool reverse) = 0;
    virtual float readNextSample(int indexNumber, bool reverse) = 0;
    virtual void writeNextSample(float sample) = 0;
    // Volumes and feedback are between 0.f and 1.f
    float calculateReadSample(float sampleIn, bool reverse, float dryVolume, float wetVolume);
    float calculateWriteSample(float sampleIn, bool reverse, float feedback);

  };

}


#endif

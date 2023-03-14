#include "AudioBuffer.h"


namespace kmodular
{

  AudioBuffer::AudioBuffer(int bufferSize)
    : writeIndex(0)
    , bufferSize(bufferSize)
    , numReadIndices(1)
  {
    for (int i = 0; i < MAX_READ_INDICES; i++) {
      readIndices[i] = 0;
      prevReadIndices[i] = 0;
      delaySizes[i] = MIN_DELAY_TIME_PARAM;
      delayLevels[i] = 0.0f;
    }
    writeInsert = 0;
    BufferManager* bufferManager = BufferManager::GetInstance();
    audioBuffer = bufferManager->RequestMediumBuffer();
  }


  AudioBuffer::~AudioBuffer() {
    BufferManager* bufferManager = BufferManager::GetInstance();
    bufferManager->ReturnMediumBuffer(audioBuffer);
  }


  // aCoefficient goes from 0.f to 1.f. Use a 0.f sampleB to fade in/out.
  float AudioBuffer::crossFade(float sampleA, float aCoefficient, float sampleB) {
    return sampleA * aCoefficient + sampleB * (1.f - aCoefficient);
  }


  void AudioBuffer::setDelaySize(int indexNumber, int numSamples) {
    prevReadIndices[indexNumber] = readIndices[indexNumber];
    delaySizes[indexNumber] = numSamples;
    readIndices[indexNumber] = writeIndex - numSamples;
    while (readIndices[indexNumber] < 0) {
        readIndices[indexNumber] += bufferSize;
    }
  }


  int AudioBuffer::getDelaySize(int indexNumber) {
      return delaySizes[indexNumber];
  }


  void AudioBuffer::setDelayLevel(int indexNumber, float level) {
      delayLevels[indexNumber] = level;
  }


  int AudioBuffer::getReadIndex(int indexNumber) {
      return readIndices[indexNumber];
  }


  void AudioBuffer::setWriteInsert(WriteInsert* writeInsertObject) {
      writeInsert = writeInsertObject;
  }


  void AudioBuffer::startLatch(int indexNumber) {
      isLatched = true;
      isFirstLatch = true;
      latchSize = delaySizes[indexNumber];
  }


  void AudioBuffer::stopLatch() {
      isLatched = false;
  }


  // Volumes are between 0.f and 1.f
  float AudioBuffer::calculateReadSample(float sampleIn, bool reverse, float dryVolume, float wetVolume) {
    float bufferSample = 0;
    for (int i = 0; i < numReadIndices; i++) {
        bufferSample += readNextSample(i, reverse) * delayLevels[i];
    }
    float returnSample = sampleIn * dryVolume + bufferSample * wetVolume;

    return returnSample;
  }


  // Feedback is between 0.0f and 1.0f
  float AudioBuffer::calculateWriteSample(float sampleIn, bool reverse, float feedback) {
    float bufferSample = 0;
    for (int i = 0; i < numReadIndices; i++) {
        bufferSample += readNextSample(i, reverse);
    }
    float returnSample = sampleIn;
    if (feedback > __FLT_EPSILON__) {
        returnSample = sampleIn + feedback * bufferSample;
    }

    if (writeInsert != 0) {
        returnSample = writeInsert->processSample(returnSample);
    }

    return returnSample;
  }

}

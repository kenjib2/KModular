#include "MultiTapDelay.h"


namespace kmodular
{

    MultiTapDelay::MultiTapDelay(int bufferSize) {
        primaryDelayBuffer = new CircularBuffer(bufferSize);
        primaryDelayBuffer->numReadIndices = 1;
        primaryDelayBuffer->setDelayLevel(0, 1.f);
        tapDelayBuffer = new CircularBuffer(bufferSize);
        tapDelayBuffer->numReadIndices = 1;
        tapDelayBuffer->setDelayLevel(0, 1.f);
    }


    MultiTapDelay::~MultiTapDelay() {
        delete primaryDelayBuffer;
        delete tapDelayBuffer;
    }


    void MultiTapDelay::setPrimaryDelayLength(int delaySamples) {
        primaryDelayBuffer->setDelaySize(0, delaySamples);
    }


    void MultiTapDelay::setPrimaryDelayLevel(float level) {
        primaryDelayBuffer->setDelayLevel(0, level);
    }


    void MultiTapDelay::setTapDelayLength(int indexNumber, int delaySamples) {
        tapDelayBuffer->setDelaySize(indexNumber, delaySamples);
    }


    void MultiTapDelay::setTapDelayLevel(int indexNumber, float level) {
        tapDelayBuffer->setDelayLevel(indexNumber, level);
    }


    void MultiTapDelay::setNumTaps(int numTaps) {
        tapDelayBuffer->numReadIndices = numTaps;
    }


    void MultiTapDelay::setWriteInsert(WriteInsert* writeInsertObject) {
        primaryDelayBuffer->setWriteInsert(writeInsertObject);
    }


    // Handle primary delay, write to buffer, then read the taps (no write)
    float MultiTapDelay::processSample(float inputSample) {
        float readSample = tapDelayBuffer->calculateReadSample(inputSample, paramReverse, paramDry, paramWet);
        float writeSample = primaryDelayBuffer->calculateWriteSample(inputSample, paramReverse, paramFeedback);
        tapDelayBuffer->writeNextSample(writeSample);

        readSample = primaryDelayBuffer->calculateReadSample(readSample, paramReverse, paramDry, paramWet);
        writeSample = primaryDelayBuffer->calculateWriteSample(inputSample, paramReverse, paramFeedback);
        primaryDelayBuffer->writeNextSample(writeSample);

        tapDelayBuffer->next(paramReverse);
        primaryDelayBuffer->next(paramReverse);

        return readSample;
    }

}

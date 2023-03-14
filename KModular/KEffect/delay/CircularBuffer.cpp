#include "CircularBuffer.h"


namespace kmodular
{

    CircularBuffer::CircularBuffer(int bufferSize)
    : AudioBuffer(bufferSize) {
    for (int i = 0; i < MAX_READ_INDICES; i++) {
        indexFadeCount[i] = 0;
    }
    }


    void CircularBuffer::incIndices(int* indices) {
        for (int i = 0; i < numReadIndices; i++) {
            indices[i]++;
            if (indices[i] >= bufferSize) {
                indices[i] -= bufferSize;
            }
        }
    }


    void CircularBuffer::incReverseIndices(int* indices) {
        for (int i = 0; i < numReadIndices; i++) {
            indices[i]--;
            if (indices[i] < 0) {
                indices[i] += bufferSize;
            }

            int relativeIndex = indices[i];
            if (relativeIndex > writeIndex) {
                relativeIndex -= bufferSize;
            }
            if (writeIndex - relativeIndex >= std::min(bufferSize - 1, delaySizes[i] * 2)) {
                indices[i] = writeIndex;
            }
        }
    }


    void CircularBuffer::next(bool reverse) {
    if (reverse) {
        incReverseIndices(readIndices);
        incReverseIndices(prevReadIndices);
    }
    else {
        incIndices(readIndices);
        incIndices(prevReadIndices);
    }
    writeIndex++;
    if (writeIndex >= bufferSize) {
        writeIndex -= bufferSize;
    }

    for (int i = 0; i < MAX_READ_INDICES; i++) {
        if (readIndices[i] != prevReadIndices[i] && indexFadeCount[i] < FADE_SAMPLES) {
            indexFadeCount[i]++;
            if (indexFadeCount[i] >= FADE_SAMPLES) {
                prevReadIndices[i] = readIndices[i];
                indexFadeCount[i] = 0;
            }
        }
    }

    // Handle latch data for writeNextSample
    if (isLatched && isFirstLatch) {
        firstLatchCount++;
        if (firstLatchCount > latchSize) {
            isFirstLatch = false;
            needPostLatchFadeIn = true;
            firstLatchCount = 0;
        }
    }
    else if (!isLatched && needPostLatchFadeIn) {
        postLatchCount++;
        if (postLatchCount >= FADE_SAMPLES) {
            postLatchCount = 0;
            needPostLatchFadeIn = false;
        }
    }
    }


    float CircularBuffer::crossFadeTwoIndices(int previousIndex, int currentIndex, int indexFadeCount) {
        float curSample = audioBuffer[currentIndex];
        float prevSample = audioBuffer[previousIndex];
        float nextSample = crossFade(prevSample, float(FADE_SAMPLES - indexFadeCount) / FADE_SAMPLES, curSample);

        return nextSample;
    }


    float CircularBuffer::readNextSample(int indexNumber, bool reverse) {
    //  static int lastSample = 0;
    float nextSample = audioBuffer[readIndices[indexNumber]];

    // Crossfade a changed index
    if (readIndices[indexNumber] != prevReadIndices[indexNumber] && indexFadeCount[indexNumber] < FADE_SAMPLES) {
        nextSample = crossFadeTwoIndices(prevReadIndices[indexNumber], readIndices[indexNumber], indexFadeCount[indexNumber]);
    }

    // Fade out/in the ends of the reversed sample
    int remaining = 0;
    if (reverse) {
        int relativeIndex = readIndices[indexNumber];
        if (relativeIndex > writeIndex) {
            relativeIndex -= bufferSize;
        }
        remaining = std::min(bufferSize - 1, delaySizes[indexNumber] * 2) - (writeIndex - relativeIndex);
        if (remaining < FADE_SAMPLES) {
            //fade out the end of a reverse delay loop
            nextSample = crossFade(nextSample, (float)remaining / FADE_SAMPLES, 0.f);
        }
        else if (writeIndex - relativeIndex < FADE_SAMPLES) {
            //fade in the beginning of a reverse delay loop
            nextSample = crossFade(nextSample, (float)(writeIndex - relativeIndex) / FADE_SAMPLES, 0.f);
        }
    }

    // Fade out the tail of the first latch so we don't get a pop
    if (isFirstLatch) {
        if (latchSize - firstLatchCount < FADE_SAMPLES) {
            nextSample = crossFade(nextSample, (float)(latchSize - firstLatchCount) / FADE_SAMPLES, 0.f);
        }
    }

    /*  if (abs(lastSample - nextSample) > 1200) {
        Serial.print("Here: ");
        Serial.print(remaining);
        Serial.print(" / ");
        Serial.print(delaySizes[indexNumber]);
        Serial.print(" -- ");
        Serial.print(prevReadIndices[indexNumber]);
        Serial.print(" : ");
        Serial.print(readIndices[indexNumber]);
        Serial.print(" -- ");
        Serial.println(indexFadeCount[indexNumber]);
    }
    lastSample = nextSample;*/
    return nextSample;
    }


    void CircularBuffer::writeNextSample(float sample) {
        float writeSample = sample;
        if (isLatched && isFirstLatch) {
            if (firstLatchCount < FADE_SAMPLES) {
                writeSample = crossFade(writeSample, (float)firstLatchCount / FADE_SAMPLES, 0.f);
            }
            else if (latchSize - firstLatchCount < FADE_SAMPLES) {
                writeSample = crossFade(writeSample, (float)(latchSize - firstLatchCount) / FADE_SAMPLES, 0.f);
            }
            audioBuffer[writeIndex] = writeSample;
        }
        else if (isLatched) {
            int latchIndex = writeIndex - latchSize;
            if (latchIndex < 0) {
                latchIndex += bufferSize;
            }
            audioBuffer[writeIndex] = audioBuffer[latchIndex];
        }
        else {
            if (needPostLatchFadeIn) {
                writeSample = crossFade(writeSample, (float)postLatchCount / FADE_SAMPLES, 0.f);
            }
            audioBuffer[writeIndex] = writeSample;
        }
    }
    
}

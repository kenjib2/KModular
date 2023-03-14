#ifndef KEFFECT_CIRCULAR_BUFFER_H
#define KEFFECT_CIRCULAR_BUFFER_H


#include <algorithm>
#include "AudioBuffer.h"


namespace kmodular
{

  class CircularBuffer : public AudioBuffer {

  protected:
    int firstLatchCount = 0;
    bool needPostLatchFadeIn = false;
    int postLatchCount = 0;

    void incIndices(int* indices);
    void incReverseIndices(int* indices);
    float crossFadeTwoIndices(int previousIndex, int currentIndex, int indexFadeCount);

  public:
    CircularBuffer(int bufferSize);
    int indexFadeCount[MAX_READ_INDICES];

    void next(bool reverse) override;
    float readNextSample(int indexNumber, bool reverse) override;
    void writeNextSample(float sample) override;
  };

}


#endif

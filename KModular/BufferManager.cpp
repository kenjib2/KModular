#include "BufferManager.h"


namespace kmodular
{

    BufferManager* BufferManager::_instance = NULL;
//    float DSY_SDRAM_BSS BufferManager::shortBuffers[NUM_SHORT][SHORT_BUFFER_SIZE];
//    float DSY_SDRAM_BSS BufferManager::mediumBuffers[NUM_MEDIUM][MEDIUM_BUFFER_SIZE];
//    float DSY_SDRAM_BSS BufferManager::longBuffers[NUM_LONG][LONG_BUFFER_SIZE];
    bool BufferManager::shortInUse[NUM_SHORT];
    bool BufferManager::mediumInUse[NUM_MEDIUM];
    bool BufferManager::longInUse[NUM_LONG];
    static float DSY_SDRAM_BSS shortBuffers[NUM_SHORT][SHORT_BUFFER_SIZE];
    static float DSY_SDRAM_BSS mediumBuffers[NUM_MEDIUM][MEDIUM_BUFFER_SIZE];
    static float DSY_SDRAM_BSS longBuffers[NUM_LONG][LONG_BUFFER_SIZE];


    BufferManager::BufferManager()
    {
        for (int i = 0; i < NUM_SHORT; i++) {
            shortInUse[i] = false;
        }
        for (int i = 0; i < NUM_MEDIUM; i++) {
            mediumInUse[i] = false;
        }
        for (int i = 0; i < NUM_LONG; i++) {
            longInUse[i] = false;
        }
    }


    float * BufferManager::RequestShortBuffer()
    {
        for (int i = 0; i < NUM_SHORT; i++) {
            if (!shortInUse[i]) {
                shortInUse[i] = true;
                return shortBuffers[i];
            }
        }

        return NULL;
    }


    float * BufferManager::RequestMediumBuffer()
    {
        for (int i = 0; i < NUM_MEDIUM; i++) {
            if (!mediumInUse[i]) {
                mediumInUse[i] = true;
                return mediumBuffers[i];
            }
        }

        return NULL;
    }


    float * BufferManager::RequestLongBuffer()
    {
        for (int i = 0; i < NUM_LONG; i++) {
            if (!longInUse[i]) {
                longInUse[i] = true;
                return longBuffers[i];
            }
        }

        return NULL;
    }


    void BufferManager::ReturnShortBuffer(float * buffer)
    {
        for (int i = 0; i < NUM_SHORT; i++) {
            if (shortBuffers[i] == buffer) {
                shortInUse[i] = false;
            }
        }
    }


    void BufferManager::ReturnMediumBuffer(float * buffer)
    {
        for (int i = 0; i < NUM_MEDIUM; i++) {
            if (mediumBuffers[i] == buffer) {
                mediumInUse[i] = false;
            }
        }
    }


    void BufferManager::ReturnLongBuffer(float * buffer)
    {
        for (int i = 0; i < NUM_LONG; i++) {
            if (longBuffers[i] == buffer) {
                longInUse[i] = false;
            }
        }
    }

}

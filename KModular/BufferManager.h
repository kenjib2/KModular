#ifndef KMODULAR_BUFFER_MANAGER
#define KMODULAR_BUFFER_MANAGER


#include "daisy_pod.h"
#include "daisysp.h"


using namespace daisy;
using namespace daisysp;


const int BUFFER_SAMPLES_PER_SEC = 48000;
const float SHORT_SECS = 0.5f;
const float MEDIUM_SECS = 3.0f;
const float LONG_SECS = 20.0f;
const int SHORT_BUFFER_SIZE = (int)(BUFFER_SAMPLES_PER_SEC * SHORT_SECS + __FLT_EPSILON__);
const int MEDIUM_BUFFER_SIZE = (int)(BUFFER_SAMPLES_PER_SEC * MEDIUM_SECS + __FLT_EPSILON__);
const int LONG_BUFFER_SIZE = (int)(BUFFER_SAMPLES_PER_SEC * LONG_SECS + __FLT_EPSILON__);
const int NUM_SHORT = 24;
const int NUM_MEDIUM = 12;
const int NUM_LONG = 6;


namespace kmodular
{
    
    class BufferManager
    {
        private:
            static BufferManager* _instance;
            static bool shortInUse[NUM_SHORT];
            static bool mediumInUse[NUM_MEDIUM];
            static bool longInUse[NUM_LONG];

            BufferManager();

        public:
            BufferManager(const BufferManager& obj) = delete;

            static BufferManager* GetInstance()
            {
                if (_instance == NULL) {
                    _instance = new BufferManager();
                }
                return _instance;
            }
            
            float* RequestShortBuffer();
            float* RequestMediumBuffer();
            float* RequestLongBuffer();

            void ReturnShortBuffer(float*);
            void ReturnMediumBuffer(float*);
            void ReturnLongBuffer(float*);
    };
}


#endif

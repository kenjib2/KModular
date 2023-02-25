#ifndef KSYNTH_PATCH_H
#define KSYNTH_PATCH_H


#include <string>
#include <vector>


using namespace std;


namespace kmodular
{
    class KSynthPatch
    {
        public:
            string patchName;
            float pitchOffset;
            float level;

            bool delayEnabled;
            float delayTime;
            float delayLevel;
            float delayFeedback;

            bool reverbEnabled;
            float reverbLevel;
            float reverbFeedback;
            float reverbLpFreq;

            int numVcos;
            vector<int> vcoWaveform;
            vector<float>  vcoLevel;
            vector<float>  vcoPitchOffset;
            vector<float>  vcoEnvAttack;
            vector<float>  vcoEnvDecay;
            vector<float>  vcoEnvSustain;
            vector<float>  vcoEnvRelease;
            vector<float>  vcoEnvDepth;
            vector<int>  vcoLfoWaveform;
            vector<float>  vcoLfoRate;
            vector<float>  vcoLfoDepth;

            float whiteNoiseOscLevel;

            float vcfFrequency;
            float vcfResonance;
            float vcfEnvAttack;
            float vcfEnvDecay;
            float vcfEnvSustain;
            float vcfEnvRelease;
            float vcfEnvDepth;
            int vcfLfoWaveform;
            float vcfLfoRate;
            float vcfLfoDepth;

            float vcaEnvAttack;
            float vcaEnvDecay;
            float vcaEnvSustain;
            float vcaEnvRelease;
            int vcaLfoWaveform;
            float vcaLfoRate;
            float vcaLfoDepth;
    };
}


#endif

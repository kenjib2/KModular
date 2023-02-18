#include "KSynth.h"


namespace kmodular
{
    
    void KSynth::Init(float sampleRate)
    {
        Init(sampleRate, DEFAULT_NUM_VOICES);
    }


    void KSynth::Init(float sampleRate, int numVoices)
    {
        for (int i = 0; i < numVoices; i++) {
            Voice nextVoice;
            nextVoice.Init(sampleRate);
            voices.push_back(nextVoice);
        }
        reverb.Init(sampleRate);
        delay.Init(sampleRate);

        Reset();
    }


    void KSynth::Reset()
    {
        level = 1.0f;
        pitchOffset = 0.0f;

        for (size_t i = 0; i < voices.size(); i++) {
            voices[i].Reset();
        }
        reverb.Reset();
        delay.Reset();
    }


    void KSynth::Process(const float* in, float* out, size_t sizeIn, size_t sizeOut)
    {
        if (sizeOut < 2) {
            return;
        }

        float result[2] = { 0.0f, 0.0f };

        for (size_t i = 0; i < voices.size(); i++) {
            float voiceOut[2];
            voices[i].Process(in, (float*)voiceOut, sizeIn, 2);
            result[0] += voiceOut[0];
            result[1] += voiceOut[1];
        }

        float delayOut[2];
        delay.Process(result, delayOut, sizeIn, 2);

        float reverbOut[2];
        reverb.Process(delayOut, reverbOut, 2, 2);

        out[0] = reverbOut[0] * level;
        out[1] = reverbOut[1] * level;
    }


    void KSynth::Trigger(TriggerCommand command, int* intVals, float* floatVals)
    {
        Voice* voice;
        switch (command)
        {
            case NoteOn:
                voice = RequestVoice(intVals[0]);
                if (voice != NULL) {
                    voice->Trigger(command, intVals, floatVals);
                }
                break;

            case NoteOff:
                for (size_t i = 0; i < voices.size(); i++) {
                    if (voices[i].noteTriggered && voices[i].currentMidiNote == intVals[0]) {
                        voices[i].Trigger(command, intVals, floatVals);
                        break;
                    }
                }
                break;

            case ParamChange:
                SynthParam synthParam = (SynthParam)intVals[0];

                // Handle voice level params
                if (synthParam >= VoiceLevel && synthParam <= VcaLfoDepth) {
                    for (size_t i = 0; i < voices.size(); i++) {
                        voices[i].Trigger(command, intVals, floatVals);
                    }

                // Delay params
                } else if (synthParam >= DelayTime && synthParam <= DelayFeedback) {
                    delay.Trigger(command, intVals, floatVals);

                // Reverb params
                } else if (synthParam >= ReverbLevel && synthParam <= ReverbLpFreq) {
                    reverb.Trigger(command, intVals, floatVals);

                // Handle patch level params
                } else {
                    switch (synthParam)
                    {
                        case Level:
                            level = floatVals[0];
                            break;

                        case PitchOffset:
                            pitchOffset = floatVals[0];
                            for (size_t i = 0; i < voices.size(); i++) {
                                voices[i].Trigger(command, intVals, floatVals);
                            }
                            break;

                        default:
                            break;
                    }
                }
                break;
        }
    }


    Voice* KSynth::RequestVoice(int midiNote)
    {
        // First if that note is already playing, retrigger it.
        for (size_t i = 0; i < voices.size(); i++) {
            if (!voices[i].IsAvailable() && voices[i].currentMidiNote == midiNote) {
                return &voices[i];
            }
        }

        // Else return first available voice.
        for (size_t i = 0; i < voices.size(); i++) {
            if (voices[i].IsAvailable()) {
                return &voices[i];
            }
        }

        // Find the lowest and highest notes to make sure that we don't return them.
        // These will be the most noticeable notes to steals.

        // If no voices are available, return the oldest note (not including lowest nor highest).
        return NULL;
    }

}

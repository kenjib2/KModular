#include "KSynth.h"


namespace kmodular
{
    
    void KSynth::Init(float sampleRate)
    {
        Init(sampleRate, DEFAULT_NUM_VOICES);
    }


    void KSynth::Init(float sampleRate, int numVoices)
    {
        this->sampleRate = sampleRate;
        this->ccSelectedVco = 1;
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
        int param[3] = { 0, 0, 0 };
        float value[1] = { 0.0f };
        SynthParam synthParam;

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
                synthParam = (SynthParam)intVals[0];

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

            case MidiControlChange:
                // intVals[0] is control number and intVals[1] is value.
                switch(intVals[0])
                {
                    case 0:
                        // Bank select.
                        break;
                    case 1:
                        // Mod wheel.
                        break;
                    case 2:
                        // Breath controller.
                        break;
                    case 4:
                        // Foot controller.
                        break;
                    case 5:
                        // Portamento time.
                        break;
                    case 6:
                        // Data entry (NRPN or RPN).
                        break;
                    case 7:
                        // Volume.
                        param[0] = (int) SynthParam::Level;
                        value[0] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 8:
                        // Balance.
                        break;
                    case 10:
                        // Pan
                        break;
                    case 11:
                        // Expression
                        break;
                    case 12:
                        // Effect Controller 1
                        break;
                    case 13:
                        // Effect Controller 2
                        break;
                    case 14:
                        // CUSTOM -- Pitch offset MSB
                        break;
                    case 15:
                        // CUSTOM -- VCO select
                        // Selects which VCO all subsequent changes will apply to
                        ccSelectedVco = intVals[1];
                        break;
                    case 16:
                        // CUSTOM -- VCO waveform
                        // Replaces General Purpose Controller 1 (MSB)
                        param[0] = (int) SynthParam::VcoWaveform;
                        param[1] = ccSelectedVco;
                        param[2] = intVals[1];
                        Trigger(ParamChange, param, value);
                        break;
                    case 17:
                        // CUSTOM -- VCO level
                        // Replaces General Purpose Controller 2 (MSB)
                        param[0] = (int) SynthParam::VcoLevel;
                        param[1] = ccSelectedVco;
                        value[0] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 18:
                        // CUSTOM -- VCO pitch offset MSB
                        // Replaces General Purpose Controller 3 (MSB)
                        break;
                    case 19:
                        // CUSTOM -- VCO envelope attack
                        // Replaces General Purpose Controller 4 (MSB)
                        param[0] = (int) SynthParam::VcoEnvAttack;
                        param[1] = ccSelectedVco;
                        value[0] = (float)intVals[1] / 127 * MAX_ATTACK_TIME;
                        Trigger(ParamChange, param, value);
                        break;
                    case 20:
                        // CUSTOM -- VCO envelope decay
                        param[0] = (int) SynthParam::VcoEnvDecay;
                        param[1] = ccSelectedVco;
                        value[0] = (float)intVals[1] / 127 * MAX_DECAY_TIME;
                        Trigger(ParamChange, param, value);
                        break;
                    case 21:
                        // CUSTOM -- VCO envelope sustain
                        param[0] = (int) SynthParam::VcoEnvSustain;
                        param[1] = ccSelectedVco;
                        value[0] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 22:
                        // CUSTOM -- VCO envelope release
                        param[0] = (int) SynthParam::VcoEnvRelease * MAX_RELEASE_TIME;
                        param[1] = ccSelectedVco;
                        value[0] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 23:
                        // CUSTOM -- VCO envelope depth MSB
                        //param[0] = (int) SynthParam::VcoEnvDepth;
                        //param[1] = ccSelectedVco;
                        //value[0] = (float)intVals[i] / 127 * MAX_VCO_ENV_DEPTH;
                        //Trigger(ParamChange, param, value);
                        break;
                    case 24:
                        // CUSTOM -- VCO LFO waveform
                        param[0] = (int) SynthParam::VcoLfoWaveform;
                        param[1] = ccSelectedVco;
                        param[2] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 25:
                        // CUSTOM -- VCO LFO rate
                        param[0] = (int) SynthParam::VcoLfoRate;
                        param[1] = ccSelectedVco;
                        value[0] = (float)intVals[1] / 127 * MAX_LFO_RATE;
                        Trigger(ParamChange, param, value);
                        break;
                    case 26:
                        // CUSTOM -- VCO LFO depth MSB
                        //param[0] = (int) SynthParam::VcoLfoDepth;
                        //param[1] = ccSelectedVco;
                        //value[0] = (float)intVals[1] / 127 * MAX_VCO_LFO_DEPTH;
                        //Trigger(ParamChange, param, value);
                        break;
                    case 27:
                        // CUSTOM -- VCO White Noise Level
                        param[0] = (int) SynthParam::WhiteNoiseOscLevel;
                        value[0] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 28:
                        // CUSTOM -- VCF Frequency MSB
                        //param[0] = (int) SynthParam::VcfFrequency;
                        //value[0] = (float)intVals[1] / 127 * (MAX_VCF_FREQUENCY - MIN_VCF_FREQUENCY) + MIN_VCF_FREQUENCY;
                        //Trigger(ParamChange, param, value);
                        break;
                    case 29:
                        // CUSTOM -- VCF Resonance
                        param[0] = (int) SynthParam::VcfResonance;
                        value[0] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 30:
                        // CUSTOM -- VCF Envelope Attack
                        param[0] = (int) SynthParam::VcfEnvAttack;
                        value[0] = (float)intVals[1] / 127 * MAX_ATTACK_TIME;
                        Trigger(ParamChange, param, value);
                        break;
                    case 31:
                        // CUSTOM -- VCF Envelope Decay
                        param[0] = (int) SynthParam::VcfEnvDecay;
                        value[0] = (float)intVals[1] / 127 * MAX_DECAY_TIME;
                        Trigger(ParamChange, param, value);
                        break;
                    case 32:
                        // CUSTOM -- VCF Envelope Sustain
                        param[0] = (int) SynthParam::VcfEnvSustain;
                        value[0] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 33:
                        // CUSTOM -- VCF Envelope Release
                        param[0] = (int) SynthParam::VcfEnvRelease;
                        value[0] = (float)intVals[1] / 127 * MAX_RELEASE_TIME;
                        Trigger(ParamChange, param, value);
                        break;
                    case 34:
                        // CUSTOM -- VCF Envelope Depth MSB
                        //param[0] = (int) SynthParam::VcfEnvDepth;
                        //value[0] = (float)intVals[1] / 127 * (MAX_VCF_FREQUENCY - MIN_VCF_FREQUENCY) + MIN_VCF_FREQUENCY;
                        //Trigger(ParamChange, param, value);
                        break;
                    case 35:
                        // CUSTOM -- VCF Envelope Depth LSB
                        break;
                    case 36:
                        // CUSTOM -- VCF LFO Waveform
                        param[0] = (int) SynthParam::VcfLfoWaveform;
                        param[1] = intVals[1];
                        Trigger(ParamChange, param, value);
                        break;
                    case 37:
                        // CUSTOM -- VCF LFO Rate
                        param[0] = (int) SynthParam::VcfLfoRate;
                        value[0] = (float)intVals[1] / 127 * MAX_LFO_RATE;
                        Trigger(ParamChange, param, value);
                        break;
                    case 38:
                        // CUSTOM -- VCF LFO Depth MSB
                        //param[0] = (int) SynthParam::VcfLfoDepth;
                        //value[0] = (float)intVals[1] / 127 * (MAX_VCF_FREQUENCY - MIN_VCF_FREQUENCY) + MIN_VCF_FREQUENCY;
                        //Trigger(ParamChange, param, value);
                        break;
                    case 39:
                        // CUSTOM -- VCF LFO Depth LSB
                        break;
                    case 40:
                        // CUSTOM -- VCA Envelope Attack
                        param[0] = (int) SynthParam::VcaEnvAttack;
                        value[0] = (float)intVals[1] / 127 * MAX_ATTACK_TIME;
                        Trigger(ParamChange, param, value);
                        break;
                    case 41:
                        // CUSTOM -- VCA Envelope Decay
                        param[0] = (int) SynthParam::VcaEnvDecay;
                        value[0] = (float)intVals[1] / 127 * MAX_DECAY_TIME;
                        Trigger(ParamChange, param, value);
                        break;
                    case 42:
                        // CUSTOM -- VCA Envelope Sustain
                        param[0] = (int) SynthParam::VcaEnvSustain;
                        value[0] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 43:
                        // CUSTOM -- VCA Envelope Release
                        param[0] = (int) SynthParam::VcaEnvRelease;
                        value[0] = (float)intVals[1] / 127 * MAX_RELEASE_TIME;
                        Trigger(ParamChange, param, value);
                        break;
                    case 44:
                        // CUSTOM -- VCA LFO Waveform
                        param[0] = (int) SynthParam::VcaLfoWaveform;
                        param[1] = intVals[1];
                        Trigger(ParamChange, param, value);
                        break;
                    case 45:
                        // CUSTOM -- VCA LFO Rate
                        param[0] = (int) SynthParam::VcaLfoRate;
                        value[0] = (float)intVals[1] / 127 * MAX_LFO_RATE;
                        Trigger(ParamChange, param, value);
                        break;
                    case 46:
                        // CUSTOM -- VCA LFO Depth
                        param[0] = (int) SynthParam::VcaLfoDepth;
                        value[0] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 47:
                        // CUSTOM -- Pitch offset LSB
                        break;
                    case 48:
                        // CUSTOM -- Delay Time
                        param[0] = (int) SynthParam::DelayTime;
                        value[0] = (float)intVals[1] / 127 * (MAX_DELAY_TIME - MIN_DELAY_TIME) + MIN_DELAY_TIME;
                        Trigger(ParamChange, param, value);
                        break;
                    case 49:
                        // CUSTOM -- Delay Level
                        param[0] = (int) SynthParam::DelayLevel;
                        value[0] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 50:
                        // CUSTOM -- VCO pitch offset LSB
                        break;
                    case 51:
                        // CUSTOM -- Delay Feedback
                        param[0] = (int) SynthParam::DelayFeedback;
                        value[0] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 52:
                        // CUSTOM -- Reverb Level
                        param[0] = (int) SynthParam::ReverbLevel;
                        value[0] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 53:
                        // CUSTOM -- Reverb Feedback
                        param[0] = (int) SynthParam::ReverbFeedback;
                        value[0] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 54:
                        // CUSTOM -- Reverb Low Pass Filter Frequency MSB
                        //param[0] = (int) SynthParam::ReverbLpFreq;
                        //value[0] = (float)intVals[1] / 127 * (MAX_VCF_FREQUENCY - MIN_VCF_FREQUENCY) + MIN_VCF_FREQUENCY;
                        //Trigger(ParamChange, param, value);
                        break;
                    case 55:
                        // CUSTOM -- VCO envelope depth LSB
                        break;
                    case 56:
                        // CUSTOM -- Reverb Low Pass Filter Frequency LSB
                        break;
                    case 58:
                        // CUSTOM -- VCO LFO depth LSB
                        break;
                    case 60:
                        // CUSTOM -- VCF Frequency LSB
                        break;                    
                    case 64:
                        // Damper pedal on/off
                        break;
                    case 65:
                        // Portamento on/off
                        break;
                    case 66:
                        // Sostenuto pedal on/off
                        break;
                    case 67:
                        // Soft pedal on/off
                        break;
                    case 68:
                        // Legato pedal on/off
                        break;
                    case 69:
                        // Hold 2
                        break;
                    case 70:
                        // Sound Controller 1: Sound variation
                        break;
                    case 71:
                        // Sound Controller 2: Resonance
                        param[0] = (int) SynthParam::VcfResonance;
                        value[0] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 72:
                        // Sound Controller 3: VCA Release Time
                        param[0] = (int) SynthParam::VcaEnvRelease;
                        value[0] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 73:
                        // Sound Controller 4: VCA Attack
                        param[0] = (int) SynthParam::VcaEnvAttack;
                        value[0] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 74:
                        // Sound Controller 5: Frequency Cutoff
                        param[0] = (int) SynthParam::VcfFrequency;
                        value[0] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 75:
                        // Sound Controller 6
                        break;
                    case 76:
                        // Sound Controller 7
                        break;
                    case 77:
                        // Sound Controller 8
                        break;
                    case 78:
                        // Sound Controller 9
                        break;
                    case 79:
                        // Sound Controller 10
                        break;
                    case 84:
                        // Portamento amount
                        break;
                    case 91:
                        // Reverb send amount
                        param[0] = (int) SynthParam::ReverbLevel;
                        value[0] = (float)intVals[1] / 127;
                        Trigger(ParamChange, param, value);
                        break;
                    case 92:
                        // Tremolo amount
                        break;
                    case 93:
                        // Chorus amount
                        break;
                    case 94:
                        // Detune amount
                        break;
                    case 95:
                        // Phaser amount
                        break;
                    case 120:
                        // All sound off without release
                        break;
                    case 121:
                        // Reset all controllers to default
                        break;
                    case 122:
                        // Local on/off
                        break;
                    case 123:
                        // All notes off (with release)
                        break;
                    case 124:
                        // Omni off
                        break;
                    case 125:
                        // Omni on
                        break;
                    case 126:
                        // Mono mode
                        break;
                    case 127:
                        // Polyphonic mode
                        break;
                    
                    default:
                        break;
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


    void KSynth::LoadPatch(KSynthPatch* patch)
    {
        int param[3] = { 0, 0, 0 };
        float value[1] = { 0.0f };

        // Synth
        param[0] = (int) SynthParam::Level;
        value[0] = patch->level;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::PitchOffset;
        value[0] = patch->pitchOffset;
        Trigger(ParamChange, param, value);

        // Voice
//        param[0] = (int) SynthParam::VoiceLevel;
//        param[1] = 1;
//        value[0] = 1.0f;
//        Trigger(ParamChange, param, value);

        // VCOs
        for (int i = 0; i < patch->numVcos; i++) {
            param[0] = (int) SynthParam::VcoWaveform;
            param[1] = i + 1;
            param[2] = patch->vcoWaveform[i];
            Trigger(ParamChange, param, value);

            param[0] = (int) SynthParam::VcoLevel;
            param[1] = i + 1;
            value[0] = patch->vcoLevel[i];
            Trigger(ParamChange, param, value);

            param[0] = (int) SynthParam::VcoPitchOffset;
            param[1] = i + 1;
            value[0] = patch->vcoPitchOffset[i];
            Trigger(ParamChange, param, value);

            param[0] = (int) SynthParam::VcoEnvAttack;
            param[1] = i + 1;
            value[0] = patch->vcoEnvAttack[i];
            Trigger(ParamChange, param, value);

            param[0] = (int) SynthParam::VcoEnvDecay;
            param[1] = i + 1;
            value[0] = patch->vcoEnvDecay[i];
            Trigger(ParamChange, param, value);

            param[0] = (int) SynthParam::VcoEnvSustain;
            param[1] = i + 1;
            value[0] = patch->vcoEnvSustain[i];
            Trigger(ParamChange, param, value);

            param[0] = (int) SynthParam::VcoEnvRelease;
            param[1] = i + 1;
            value[0] = patch->vcoEnvRelease[i];
            Trigger(ParamChange, param, value);

            param[0] = (int) SynthParam::VcoEnvDepth;
            param[1] = i + 1;
            value[0] = patch->vcoEnvRelease[i];
            Trigger(ParamChange, param, value);

            param[0] = (int) SynthParam::VcoLfoWaveform;
            param[1] = i + 1;
            param[2] = patch->vcoLfoWaveform[i];
            Trigger(ParamChange, param, value);

            param[0] = (int) SynthParam::VcoLfoRate;
            param[1] = i + 1;
            value[0] = patch->vcoLfoRate[i];
            Trigger(ParamChange, param, value);

            param[0] = (int) SynthParam::VcoLfoDepth;
            param[1] = i + 1;
            value[0] = patch->vcoLfoDepth[i];
            Trigger(ParamChange, param, value);
        }

        // White Noise
        param[0] = (int) SynthParam::WhiteNoiseOscLevel;
        value[0] = patch->whiteNoiseOscLevel;
        Trigger(ParamChange, param, value);

        // VCF
        param[0] = (int) SynthParam::VcfFrequency;
        value[0] = patch->vcfFrequency;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::VcfResonance;
        value[0] = patch->vcfResonance;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::VcfEnvAttack;
        value[0] = patch->vcfEnvAttack;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::VcfEnvDecay;
        value[0] = patch->vcfEnvDecay;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::VcfEnvSustain;
        value[0] = patch->vcfEnvSustain;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::VcfEnvRelease;
        value[0] = patch->vcfEnvRelease;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::VcfEnvDepth;
        value[0] = patch->vcfEnvDepth;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::VcfLfoWaveform;
        param[1] = patch->vcfLfoWaveform;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::VcfLfoRate;
        value[0] = patch->vcfLfoRate;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::VcfLfoDepth;
        value[0] = patch->vcfLfoDepth;
        Trigger(ParamChange, param, value);

        // VCA
        param[0] = (int) SynthParam::VcaEnvAttack;
        value[0] = patch->vcaEnvAttack;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::VcaEnvDecay;
        value[0] = patch->vcaEnvDecay;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::VcaEnvSustain;
        value[0] = patch->vcaEnvSustain;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::VcaEnvRelease;
        value[0] = patch->vcaEnvRelease;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::VcaLfoWaveform;
        param[1] = patch->vcaLfoWaveform;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::VcaLfoRate;
        value[0] = patch->vcaLfoRate;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::VcaLfoDepth;
        value[0] = patch->vcaLfoDepth;
        Trigger(ParamChange, param, value);

        // Delay
        param[0] = (int) SynthParam::DelayTime;
        value[0] = patch->delayTime * sampleRate;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::DelayLevel;
        value[0] = patch->delayLevel;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::DelayFeedback;
        value[0] = patch->delayFeedback;
        Trigger(ParamChange, param, value);

        // Reverb
        param[0] = (int) SynthParam::ReverbLevel;
        value[0] = patch->reverbLevel;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::ReverbFeedback;
        value[0] = patch->reverbFeedback;
        Trigger(ParamChange, param, value);

        param[0] = (int) SynthParam::ReverbLpFreq;
        value[0] = patch->reverbLpFreq;
        Trigger(ParamChange, param, value);
    }
}

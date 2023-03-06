#ifndef KMODULAR_AUDIO_MODULE
#define KMODULAR_AUDIO_MODULE


namespace kmodular
{

    typedef enum TriggerCommand
    {
        TriggerNoteOn,
        TriggerNoteOff,
        TriggerParamChange,
        TriggerMidiControlChange,
    } TriggerCommand;


    typedef enum SynthParam
    {
        PatchName,
        PitchOffset,
        Level,

        ChorusAmount,
        ChorusDelay,
        ChorusFeedback,
        ChorusLfoFreq,
        ChorusLfoDepth,

        DelayTime,
        DelayLevel,
        DelayFeedback,

        ReverbLevel,
        ReverbFeedback,
        ReverbLpFreq,

        VoiceLevel,

        VcoWaveform,
        VcoLevel,
        VcoPitchOffset,
        VcoEnvAttack,
        VcoEnvDecay,
        VcoEnvSustain,
        VcoEnvRelease,
        VcoEnvDepth,
        VcoLfoWaveform,
        VcoLfoRate,
        VcoLfoDepth,

        WhiteNoiseOscLevel,

        VcfFrequency,
        VcfResonance,
        VcfEnvAttack,
        VcfEnvDecay,
        VcfEnvSustain,
        VcfEnvRelease,
        VcfEnvDepth,
        VcfLfoWaveform,
        VcfLfoRate,
        VcfLfoDepth,

        VcaEnvAttack,
        VcaEnvDecay,
        VcaEnvSustain,
        VcaEnvRelease,
        VcaLfoWaveform,
        VcaLfoRate,
        VcaLfoDepth,
    } SynthParam;


    class AudioModule {
        public:
            int midiChannel;
            virtual void Init(float sampleRate) = 0; // Runs only once when first starting up.
            virtual void Reset() = 0; // Sets it back to default values.
            virtual void Process(const float* in, float* out, size_t sizeIn = 2, size_t sizeOut = 2) = 0; // Process one sample.
            virtual void Trigger(TriggerCommand command, int* intVals, float* floatVals) = 0; // Send a command.
    };

}


#endif

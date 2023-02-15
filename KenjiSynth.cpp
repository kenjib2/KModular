#include "daisy_pod.h"
#include "daisysp.h"
#include "KModular/KSynth/KSynth.h"
#include "KModular/AudioModule.h"
#include "KModular/KEffect/DelayModule.h"


using namespace daisy;
using namespace daisysp;
using namespace kmodular;


const int NUM_VOICES = 2;
float volume = 1.0f;

DaisyPod    hw;
KSynth      synth;
Parameter   p_freq;
Parameter   p_delay;


// TODO
// Add pitch offset for VCOs -- for detune -- Default patch is vcos[1].baseFreq = vcos[0].baseFreq * 0.993f
// Add trigger changes for voice and KSynth levels
// Add voice stealing -- replace oldest note, never replace bottom note.
// envelope depth for VCO/VCF/VCA
// figure out if envelopes and LFOs should add, subtract, or modulate from center
// make VCO/VCA/VCF mods logorithmic?


void InitTestPatch();


void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    hw.ProcessDigitalControls();

    if(hw.button1.RisingEdge())
    {
		int intVals[2] = { 40, 127 };
        synth.Trigger(TriggerCommand::NoteOn, intVals, NULL);
    }
    if(hw.button1.FallingEdge())
    {
		int intVals[1] = { 40 };
        synth.Trigger(TriggerCommand::NoteOff, intVals, NULL);
    }

    if(hw.button2.RisingEdge())
    {
		int intVals[2] = { 56, 127 };
        synth.Trigger(TriggerCommand::NoteOn, intVals, NULL);
    }
    if(hw.button2.FallingEdge())
    {
		int intVals[1] = { 56 };
        synth.Trigger(TriggerCommand::NoteOff, intVals, NULL);
    }

    int cutoffParam[1] = { (int) SynthParam::VcfFrequency };
    float cutoff[1] = { p_freq.Process() };
    synth.Trigger(ParamChange, cutoffParam, cutoff);

//    int resonanceParam[1] = { (int) SynthParam::VcfResonance };
//    float resonance[1] = { p_res.Process() };
//    synth.Trigger(ParamChange, resonanceParam, resonance);
    int delayParam[1] = { (int) SynthParam::DelayTime };
    float delayTime[1] = { p_delay.Process() };
    synth.Trigger(ParamChange, delayParam, delayTime);

	for (size_t i = 0; i < size; i++)
	{
		float outSamples[2];
		synth.Process(in[0], outSamples, 2, 2);
		out[0][i] = outSamples[0] * volume;
		out[1][i] = outSamples[1] * volume;
	}
}


int main(void)
{
	hw.Init();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

	synth.Init(hw.AudioSampleRate(), NUM_VOICES);
    InitTestPatch();
    p_freq.Init(hw.knob1, 10.0f, 12000.0f, Parameter::LOGARITHMIC);
    p_delay.Init(hw.knob2, hw.AudioSampleRate() * .05, MAX_DELAY, Parameter::LOGARITHMIC);

    hw.StartAdc();
	hw.StartAudio(AudioCallback);

	while(1) {}
}


void InitTestPatch()
{
    int param[3] = { 0, 0, 0 };
    float value[1] = { 0.0f };

    // VCO 1
    param[0] = (int) SynthParam::VcoWaveform;
    param[1] = 0;
    param[2] = (int) Oscillator::WAVE_POLYBLEP_SAW;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcoLevel;
    param[1] = 0;
    value[0] = 0.7f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcoEnvAttack;
    param[1] = 0;
    value[0] = 0.002f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcoEnvDecay;
    param[1] = 0;
    value[0] = 0.0f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcoEnvSustain;
    param[1] = 0;
    value[0] = 1.0f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcoEnvRelease;
    param[1] = 0;
    value[0] = 0.002f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcoEnvDepth;
    param[1] = 0;
    value[0] = 0.0f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcoLfoWaveform;
    param[1] = 0;
    param[2] = (int) Oscillator::WAVE_TRI;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcoLfoRate;
    param[1] = 0;
    value[0] = 3.3f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcoLfoDepth;
    param[1] = 0;
    value[0] = 0.03f;
    synth.Trigger(ParamChange, param, value);

    // VCO 2
    param[0] = (int) SynthParam::VcoWaveform;
    param[1] = 1;
//    param[2] = (int) Oscillator::WAVE_POLYBLEP_SAW;
    param[2] = (int) Oscillator::WAVE_SAW;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcoLevel;
    param[1] = 1;
    value[0] = 0.7f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcoEnvAttack;
    param[1] = 1;
    value[0] = 0.002f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcoEnvDecay;
    param[1] = 1;
    value[0] = 0.0f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcoEnvSustain;
    param[1] = 1;
    value[0] = 1.0f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcoEnvRelease;
    param[1] = 1;
    value[0] = 0.002f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcoEnvDepth;
    param[1] = 1;
    value[0] = 0.0f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcoLfoWaveform;
    param[1] = 1;
    param[2] = (int) Oscillator::WAVE_TRI;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcoLfoRate;
    param[1] = 1;
    value[0] = 3.3f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcoLfoDepth;
    param[1] = 1;
    value[0] = 0.03f;
    synth.Trigger(ParamChange, param, value);

    // VCF
    param[0] = (int) SynthParam::VcfFrequency;
    value[0] = 800.0f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcfResonance;
    value[0] = 0.5f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcfEnvAttack;
    value[0] = 0.6f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcfEnvDecay;
    value[0] = 0.2f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcfEnvSustain;
    value[0] = 1.0f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcfEnvRelease;
    value[0] = 0.3f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcfEnvDepth;
    value[0] = 1.0f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcfLfoWaveform;
    param[1] = (int) Oscillator::WAVE_TRI;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcfLfoRate;
    value[0] = 0.3f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcfLfoDepth;
    value[0] = 0.3f;
    synth.Trigger(ParamChange, param, value);

    // VCA
    param[0] = (int) SynthParam::VcaEnvAttack;
    value[0] = 0.3f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcaEnvDecay;
    value[0] = 0.2f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcaEnvSustain;
    value[0] = 0.3f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcaEnvRelease;
    value[0] = 0.3f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcaLfoWaveform;
    param[1] = (int) Oscillator::WAVE_TRI;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcaLfoRate;
    value[0] = 1.5f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::VcaLfoDepth;
    value[0] = 0.3f;
    synth.Trigger(ParamChange, param, value);

    // Delay
    param[0] = (int) SynthParam::DelayTime;
    value[0] = 0.5f * hw.AudioSampleRate();
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::DelayLevel;
    value[0] = 0.5f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::DelayFeedback;
    value[0] = 0.3f;
    synth.Trigger(ParamChange, param, value);

    // Reverb
    param[0] = (int) SynthParam::ReverbLevel;
    value[0] = 0.35f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::ReverbFeedback;
    value[0] = 0.82f;
    synth.Trigger(ParamChange, param, value);

    param[0] = (int) SynthParam::ReverbLpFreq;
    value[0] = 12000.0f;
    synth.Trigger(ParamChange, param, value);

}

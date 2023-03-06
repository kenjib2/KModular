#include "daisy_pod.h"
#include "daisysp.h"
#include "KModular/KSynth/KSynth.h"
#include "KModular/KSynth/KSynthPatch.h"
#include "KModular/AudioModule.h"
#include "KModular/KEffect/DelayModule.h"
#include "KModular/MidiTrigger.h"


#define DEBUG


using namespace daisy;
using namespace daisysp;
using namespace kmodular;


const int NUM_VOICES = 4;
#ifdef DEBUG
    const int MONITOR_CYCLES = 100;
    int monitorCycle = 0;
#endif
float volume = 1.0f;

DaisyPod    hw;
KSynth      synth;
Limiter     limiterL;
Limiter     limiterR;
MidiTrigger midiTrigger;
Parameter knob1;
Parameter knob2;
#ifdef DEBUG
    CpuLoadMeter cpuLoadMeter;
#endif


// TODO
// Voice level isn't working when less than 1.0f
// With patch2 some voices don't play on subsequent notes -- something to do with retriggering envelope? If you hold down a note and play a second note after decay, the 2nd note wont sound.
// Figure out my some NoteOff signals don't get through
// make VCO/VCA/VCF mods logorithmic?
// Only listen to set midi channel
// Add velocity to NoteOn
// Look into clipping issues
// Add pan/balance
// Implement access to all functions via MIDI
// Make some MIDI CC translations in KSynth scale logorithmically


void InitTestPatch();
void InitTestPatch2();


void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
#ifdef DEBUG
  	cpuLoadMeter.OnBlockStart();
#endif

    hw.ProcessDigitalControls();

    if(hw.button1.RisingEdge())
    {
		int intVals[2] = { 40, 127 };
        synth.Trigger(TriggerCommand::TriggerNoteOn, intVals, NULL);
    }
    if(hw.button1.FallingEdge())
    {
		int intVals[1] = { 40 };
        synth.Trigger(TriggerCommand::TriggerNoteOff, intVals, NULL);
    }

    if(hw.button2.RisingEdge())
    {
		int intVals[2] = { 56, 127 };
        synth.Trigger(TriggerCommand::TriggerNoteOn, intVals, NULL);
    }
    if(hw.button2.FallingEdge())
    {
		int intVals[1] = { 56 };
        synth.Trigger(TriggerCommand::TriggerNoteOff, intVals, NULL);
    }

/*    float floatVals[1];
    int intVals[1];
    intVals[0] = SynthParam::VcfFrequency;
    floatVals[0] = knob1.Process();
    synth.Trigger(TriggerCommand::TriggerParamChange, intVals, floatVals);

    intVals[0] = SynthParam::VcfResonance;
    floatVals[0] = knob2.Process();
    synth.Trigger(TriggerCommand::TriggerParamChange, intVals, floatVals);*/

	for (size_t i = 0; i < size; i++)
	{
		float outSamples[2];
		synth.Process(in[0], outSamples, 2, 2);
		out[0][i] = outSamples[0] * volume;
		out[1][i] = outSamples[1] * volume;
	}

    limiterL.ProcessBlock(out[0], size, 1.0f);
    limiterR.ProcessBlock(out[1], size, 1.0f);

#ifdef DEBUG
	cpuLoadMeter.OnBlockEnd();

	monitorCycle++;	
	if (monitorCycle >= MONITOR_CYCLES) {
		int min = cpuLoadMeter.GetMinCpuLoad() * 100;
		int avg = cpuLoadMeter.GetAvgCpuLoad() * 100;
		int max = cpuLoadMeter.GetMaxCpuLoad() * 100;
		cpuLoadMeter.Reset();
		monitorCycle = 0;
        hw.seed.PrintLine("Min: %d, Avg: %d, Max: %d", min, avg, max);
	}
#endif
}


int main(void)
{
	hw.Init();
	hw.SetAudioBlockSize(128); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

    knob1.Init(hw.knob1, 80, 18000, Parameter::LOGARITHMIC);
    knob2.Init(hw.knob2, 0, 1, Parameter::LINEAR);

	synth.Init(hw.AudioSampleRate(), &hw, NUM_VOICES);
    InitTestPatch();

    limiterL.Init();
    limiterR.Init();

    midiTrigger.Init(&hw);
    midiTrigger.AddMidiListener(&synth);

    #ifdef DEBUG
        hw.seed.StartLog(false);
    	cpuLoadMeter.Init(hw.AudioSampleRate(), hw.AudioBlockSize());
    #endif

    hw.StartAdc();
	hw.StartAudio(AudioCallback);

	while(1)
    {
        midiTrigger.Process();
    }
}


void InitTestPatch()
{
    KSynthPatch patch;
    patch.patchName = "TestPatch";
    patch.pitchOffset = 0.0f;
    patch.level = 1.0f;

    patch.chorusAmount = 0.f;
    patch.chorusDelay = 0.75f;
    patch.chorusFeedback = 0.2f;
    patch.chorusLfoFreq = 0.3f;
    patch.chorusLfoDepth = 0.9f;

    patch.delayTime = 0.5f;
    patch.delayLevel = 0.5f;
    patch.delayFeedback = 0.3f;

    patch.reverbLevel = 0.35f;
    patch.reverbFeedback = 0.82f;
    patch.reverbLpFreq = 12000.0f;

    patch.numVcos = 2;

    patch.vcoWaveform.push_back((int) Oscillator::WAVE_POLYBLEP_SAW);
    patch.vcoLevel.push_back(0.5f);
    patch.vcoPitchOffset.push_back(0.0f);
    patch.vcoEnvAttack.push_back(0.002f);
    patch.vcoEnvDecay.push_back(0.0f);
    patch.vcoEnvSustain.push_back(1.0f);
    patch.vcoEnvRelease.push_back(0.002f);
    patch.vcoEnvDepth.push_back(0.0f);
    patch.vcoLfoWaveform.push_back((int) Oscillator::WAVE_TRI);
    patch.vcoLfoRate.push_back(3.3f);
    patch.vcoLfoDepth.push_back(0.03f);

    patch.vcoWaveform.push_back((int) Oscillator::WAVE_POLYBLEP_SAW);
    patch.vcoLevel.push_back(0.5f);
    patch.vcoPitchOffset.push_back(-0.007f);
    patch.vcoEnvAttack.push_back(0.002f);
    patch.vcoEnvDecay.push_back(0.0f);
    patch.vcoEnvSustain.push_back(1.0f);
    patch.vcoEnvRelease.push_back(0.002f);
    patch.vcoEnvDepth.push_back(0.0f);
    patch.vcoLfoWaveform.push_back((int) Oscillator::WAVE_TRI);
    patch.vcoLfoRate.push_back(3.3f);
    patch.vcoLfoDepth.push_back(0.03f);

    patch.whiteNoiseOscLevel = 0.0f;

    patch.vcfFrequency = 100.0f;
    patch.vcfResonance = 0.5f;
    patch.vcfEnvAttack = 0.6f;
    patch.vcfEnvDecay = 0.2f;
    patch.vcfEnvSustain = 1.0f;
    patch.vcfEnvRelease = 0.3f;
    patch.vcfEnvDepth = 0.06f;
    patch.vcfLfoWaveform = (int) Oscillator::WAVE_TRI;
    patch.vcfLfoRate = 0.3f;
    patch.vcfLfoDepth = 0.3f;

    patch.vcaEnvAttack = 0.3f;
    patch.vcaEnvDecay = 0.2f;
    patch.vcaEnvSustain = 0.3f;
    patch.vcaEnvRelease = 0.3f;
    patch.vcaLfoWaveform = (int) Oscillator::WAVE_TRI;
    patch.vcaLfoRate = 1.5f;
    patch.vcaLfoDepth = 0.3f;

    synth.LoadPatch(&patch);
}


void InitTestPatch2()
{
    KSynthPatch patch;
    patch.patchName = "TestPatch2";
    patch.pitchOffset = 0.0f;
    patch.level = 0.5f;

    patch.chorusAmount = 0.0f;
    patch.chorusDelay = 0.75f;
    patch.chorusFeedback = 0.2f;
    patch.chorusLfoFreq = 0.3f;
    patch.chorusLfoDepth = 0.9f;

    patch.delayTime = 0.25f;
    patch.delayLevel = 0.3f;
    patch.delayFeedback = 0.2f;

    patch.reverbLevel = 0.35f;
    patch.reverbFeedback = 0.82f;
    patch.reverbLpFreq = 12000.0f;

    patch.numVcos = 2;

    patch.vcoWaveform.push_back((int) Oscillator::WAVE_POLYBLEP_SQUARE);
    patch.vcoLevel.push_back(0.5f);
    patch.vcoPitchOffset.push_back(0.0f);
    patch.vcoEnvAttack.push_back(0.002f);
    patch.vcoEnvDecay.push_back(0.0f);
    patch.vcoEnvSustain.push_back(1.0f);
    patch.vcoEnvRelease.push_back(0.002f);
    patch.vcoEnvDepth.push_back(0.0f);
    patch.vcoLfoWaveform.push_back((int) Oscillator::WAVE_TRI);
    patch.vcoLfoRate.push_back(3.3f);
    patch.vcoLfoDepth.push_back(0.0f);

    patch.vcoWaveform.push_back((int) Oscillator::WAVE_POLYBLEP_SQUARE);
    patch.vcoLevel.push_back(0.5f);
    patch.vcoPitchOffset.push_back(-0.007f);
    patch.vcoEnvAttack.push_back(0.002f);
    patch.vcoEnvDecay.push_back(0.0f);
    patch.vcoEnvSustain.push_back(1.0f);
    patch.vcoEnvRelease.push_back(0.002f);
    patch.vcoEnvDepth.push_back(0.0f);
    patch.vcoLfoWaveform.push_back((int) Oscillator::WAVE_TRI);
    patch.vcoLfoRate.push_back(3.3f);
    patch.vcoLfoDepth.push_back(0.0f);

    patch.whiteNoiseOscLevel = 0.0f;

    patch.vcfFrequency = 400.0f;
    patch.vcfResonance = 0.5f;
    patch.vcfEnvAttack = 0.04f;
    patch.vcfEnvDecay = 0.0f;
    patch.vcfEnvSustain = 1.0f;
    patch.vcfEnvRelease = 0.3f;
    patch.vcfEnvDepth = 0.06f;
    patch.vcfLfoWaveform = (int) Oscillator::WAVE_TRI;
    patch.vcfLfoRate = 0.3f;
    patch.vcfLfoDepth = 0.0f;

    patch.vcaEnvAttack = 0.002f;
    patch.vcaEnvDecay = 0.1f;
    patch.vcaEnvSustain = 0.0f;
    patch.vcaEnvRelease = 0.1f;
    patch.vcaLfoWaveform = (int) Oscillator::WAVE_TRI;
    patch.vcaLfoRate = 1.5f;
    patch.vcaLfoDepth = 0.0f;

    synth.LoadPatch(&patch);
}

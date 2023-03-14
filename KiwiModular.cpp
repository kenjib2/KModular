#include "daisy_pod.h"
#include "daisysp.h"
#include "KModular/AudioModule.h"
#include "KModular/BufferManager.h"
#include "KModular/MidiTrigger.h"
#include "KModular/KSynth/KSynth.h"
#include "KModular/KSynth/KSynthPatch.h"


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
// To move delay buffers into SDRAM, will have to get rid of malloc. So an array of fixed buffers and then each instance needs to keep track of which buffer it is using.
// Voice level isn't working when less than 1.0f
// With patch2 some voices don't play on subsequent notes -- something to do with retriggering envelope? If you hold down a note and play a second note after decay, the 2nd note wont sound.
// Patch2 craps out after playing for a while. Is something weird going on with phase cancellation?
// MIDI input is messed up. Both extra notes in and notes not cutting off.
// make VCO/VCA/VCF mods logorithmic?
// Only listen to set midi channel
// Add velocity to NoteOn
// Add pan/balance
// Implement access to all functions via MIDI
// Make some MIDI CC translations in KSynth scale logorithmically


void InitTestPatch();
void InitTestPatch2();
void InitTestPatch3();
void InitTestPatch4();


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
	hw.SetAudioBlockSize(48); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

    knob1.Init(hw.knob1, 80, 18000, Parameter::LOGARITHMIC);
    knob2.Init(hw.knob2, 0, 1, Parameter::LINEAR);

	synth.Init(hw.AudioSampleRate(), &hw, NUM_VOICES);
    InitTestPatch2();
    // Create the BufferManager singleton instance in advance.
    [[maybe_unused]] BufferManager* bufferManager = BufferManager::GetInstance();

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


// More names: Memory, Ponderous, Immensity, Gravity, Horizon, Tidal, Solitude, Sundown, Antares, Nebula, Constellation, Atmosphere
void InitTestPatch()
{
    KSynthPatch patch;
    patch.patchName = "Daybreak"; // Modwheel needs to go to VCF cutoff on this one!
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

    patch.vcoWaveform.push_back((int) Oscillator::WAVE_SAW);
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

    patch.vcoWaveform.push_back((int) Oscillator::WAVE_SAW);
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
    patch.patchName = "Ponder";
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

    patch.vcoWaveform.push_back((int) Oscillator::WAVE_SQUARE);
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

    patch.vcoWaveform.push_back((int) Oscillator::WAVE_SQUARE);
    patch.vcoLevel.push_back(0.5f);
    patch.vcoPitchOffset.push_back(-0.06f);
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
    patch.vcaLfoRate = 0.3f;
    patch.vcaLfoDepth = 0.0f;

    synth.LoadPatch(&patch);
}


void InitTestPatch3()
{
    KSynthPatch patch;
    patch.patchName = "Rewind";
    patch.pitchOffset = 0.0f;
    patch.level = 0.5f;

    patch.chorusAmount = 0.0f;
    patch.chorusDelay = 0.75f;
    patch.chorusFeedback = 0.2f;
    patch.chorusLfoFreq = 0.3f;
    patch.chorusLfoDepth = 0.9f;

    patch.delayTime = 0.25f;
    patch.delayLevel = 0.0f;
    patch.delayFeedback = 0.2f;

    patch.reverbLevel = 0.5f;
    patch.reverbFeedback = 0.85f;
    patch.reverbLpFreq = 12000.0f;

    patch.numVcos = 2;

    patch.vcoWaveform.push_back((int) Oscillator::WAVE_SAW);
    patch.vcoLevel.push_back(0.5f);
    patch.vcoPitchOffset.push_back(0.0f);
    patch.vcoEnvAttack.push_back(0.5f);
    patch.vcoEnvDecay.push_back(0.0f);
    patch.vcoEnvSustain.push_back(1.0f);
    patch.vcoEnvRelease.push_back(0.002f);
    patch.vcoEnvDepth.push_back(0.0f);
    patch.vcoLfoWaveform.push_back((int) Oscillator::WAVE_TRI);
    patch.vcoLfoRate.push_back(0.3f);
    patch.vcoLfoDepth.push_back(0.01f);

    patch.vcoWaveform.push_back((int) Oscillator::WAVE_SAW);
    patch.vcoLevel.push_back(0.5f);
    patch.vcoPitchOffset.push_back(0.0f);
    patch.vcoEnvAttack.push_back(0.3f);
    patch.vcoEnvDecay.push_back(0.0f);
    patch.vcoEnvSustain.push_back(1.0f);
    patch.vcoEnvRelease.push_back(4.0f);
    patch.vcoEnvDepth.push_back(1.0f);
    patch.vcoLfoWaveform.push_back((int)Oscillator::WAVE_TRI);
    patch.vcoLfoRate.push_back(0.3f);
    patch.vcoLfoDepth.push_back(0.0f);

    patch.whiteNoiseOscLevel = 0.0f;

    patch.vcfFrequency = 1000.0f;
    patch.vcfResonance = 0.2f;
    patch.vcfEnvAttack = 2.0f;
    patch.vcfEnvDecay = 0.0f;
    patch.vcfEnvSustain = 1.0f;
    patch.vcfEnvRelease = 0.2f;
    patch.vcfEnvDepth = 0.7f;
    patch.vcfLfoWaveform = (int) Oscillator::WAVE_TRI;
    patch.vcfLfoRate = 0.3f;
    patch.vcfLfoDepth = 0.0f;

    patch.vcaEnvAttack = 0.08f;
    patch.vcaEnvDecay = 0.0f;
    patch.vcaEnvSustain = 1.0f;
    patch.vcaEnvRelease = 0.2f;
    patch.vcaLfoWaveform = (int) Oscillator::WAVE_TRI;
    patch.vcaLfoRate = 0.3f;
    patch.vcaLfoDepth = 0.0f;

    synth.LoadPatch(&patch);
}


void InitTestPatch4()
{
    KSynthPatch patch;
    patch.patchName = "Mourn";
    patch.pitchOffset = 0.0f;
    patch.level = 0.5f;

    patch.chorusAmount = 0.0f;
    patch.chorusDelay = 0.75f;
    patch.chorusFeedback = 0.2f;
    patch.chorusLfoFreq = 0.3f;
    patch.chorusLfoDepth = 0.9f;

    patch.delayTime = 0.45f;
    patch.delayLevel = 0.30f;
    patch.delayFeedback = 0.2f;

    patch.reverbLevel = 0.0f;
    patch.reverbFeedback = 0.85f;
    patch.reverbLpFreq = 12000.0f;

    patch.numVcos = 2;

    patch.vcoWaveform.push_back((int) Oscillator::WAVE_SAW);
    patch.vcoLevel.push_back(0.5f);
    patch.vcoPitchOffset.push_back(0.0f);
    patch.vcoEnvAttack.push_back(0.5f);
    patch.vcoEnvDecay.push_back(0.0f);
    patch.vcoEnvSustain.push_back(1.0f);
    patch.vcoEnvRelease.push_back(0.002f);
    patch.vcoEnvDepth.push_back(0.0f);
    patch.vcoLfoWaveform.push_back((int) Oscillator::WAVE_TRI);
    patch.vcoLfoRate.push_back(0.3f);
    patch.vcoLfoDepth.push_back(0.01f);

    patch.vcoWaveform.push_back((int) Oscillator::WAVE_SAW);
    patch.vcoLevel.push_back(0.5f);
    patch.vcoPitchOffset.push_back(-12.0f);
    patch.vcoEnvAttack.push_back(2.0f);
    patch.vcoEnvDecay.push_back(0.0f);
    patch.vcoEnvSustain.push_back(1.0f);
    patch.vcoEnvRelease.push_back(6.0f);
    patch.vcoEnvDepth.push_back(1.0f);
    patch.vcoLfoWaveform.push_back((int)Oscillator::WAVE_TRI);
    patch.vcoLfoRate.push_back(0.3f);
    patch.vcoLfoDepth.push_back(0.0f);

    patch.whiteNoiseOscLevel = 0.0f;

    patch.vcfFrequency = 3600.0f;
    patch.vcfResonance = 0.1f;
    patch.vcfEnvAttack = 0.002f;
    patch.vcfEnvDecay = 0.0f;
    patch.vcfEnvSustain = 1.0f;
    patch.vcfEnvRelease = 0.002f;
    patch.vcfEnvDepth = 0.0f;
    patch.vcfLfoWaveform = (int) Oscillator::WAVE_TRI;
    patch.vcfLfoRate = 0.3f;
    patch.vcfLfoDepth = 0.0f;

    patch.vcaEnvAttack = 1.3f;
    patch.vcaEnvDecay = 0.0f;
    patch.vcaEnvSustain = 1.0f;
    patch.vcaEnvRelease = 0.8f;
    patch.vcaLfoWaveform = (int) Oscillator::WAVE_TRI;
    patch.vcaLfoRate = 5.0f;
    patch.vcaLfoDepth = 0.09f;

    synth.LoadPatch(&patch);
}

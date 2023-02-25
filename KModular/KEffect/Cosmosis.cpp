#include "Cosmosis.h"


namespace kmodular
{
//	Cosmosis::~Cosmosis() {
	/*	for (int i = 0; i < NUM_COSMOSIS_BUFFERS; i++) {
			delete delayBuffer[i];
			delete delayModulation[i];
		}*/
//	}


	void Cosmosis::Init(float sampleRate)
	{
	//	usesSynthMidi = false;

	//	setEffectName("Cosmosis");

	//	numButtons = 0;

	/*	numKnobs = 16;
		setKnobLabel(0, "Mix"); // Crossfade between dry and wet
		setKnobLabel(1, "Tim1");
		setKnobLabel(2, "Lvl1");
		setKnobLabel(3, "Dep1");
		setKnobLabel(4, "Spd1");
		setKnobLabel(5, "Dpl1"); // Doppler for Modulation
		setKnobLabel(6, "Tim2");
		setKnobLabel(7, "Tlv2");
		setKnobLabel(8, "Dep2");
		setKnobLabel(9, "Spd2");
		setKnobLabel(10, "Dpl2");
		setKnobLabel(11, "Tim3");
		setKnobLabel(12, "Tlv3");
		setKnobLabel(13, "Dep3");
		setKnobLabel(14, "Spd3");
		setKnobLabel(15, "Dpl3");

		numSwitches = 0;

		for (int i = 0; i < NUM_COSMOSIS_BUFFERS; i++) {
			delayLengths[i] = 7800 - i * 773;
			delayBuffer[i] = new Delay(MAX_COSMOSIS_DELAY);

			delayBuffer[i]->setDelayLength(delayLengths[i]);

			delayBuffer[i]->paramReverse = false;
			delayBuffer[i]->paramDry = paramDryLevel;
			delayBuffer[i]->paramWet = paramWetLevel;
			delayBuffer[i]->paramFeedback = 0.f;

			delayModulation[i] = new Modulation();
			delayModulation[i]->depth = 0.558f;
			delayModulation[i]->doppler = -0.2f;
	//		delayModulation[i]->doppler = -1.f;
			delayModulation[i]->setRate(1.f + .3 * i);
			delayModulation[i]->setWaveform(COSMOSIS_LFO_WAVEFORM);
			delayBuffer[i]->setWriteInsert(delayModulation[i]);
			//        floatIndex += (lfoValue + 1) / 88 + 0.99; // Target for Modulation.h params
		}
	*/
	}


	void Cosmosis::Reset()
	{
	}


	//void Cosmosis::processEffect(int16_t* effectBuffer, Controls* controls) {
	void Cosmosis::Process(const float* in, float* out, size_t sizeIn, size_t sizeOut) {
	/*	for (int i = 0; i < 128; i++) {
			int16_t nextSample = effectBuffer[i];
			for (int i = 0; i < NUM_COSMOSIS_BUFFERS; i++) {
				nextSample = delayBuffer[i]->processSample(nextSample);
			}
			effectBuffer[i] = nextSample;
		}*/
	}


	void Cosmosis::Trigger(TriggerCommand command, int* intVals, float* floatVals)
	{
	}

}

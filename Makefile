# Project Name
TARGET = KenjiSynth

# Sources
CPP_SOURCES = KiwiModular.cpp KModular/MidiTrigger.cpp KModular/BufferManager.cpp KModular/KSynth/KSynth.cpp KModular/KSynth/Voice.cpp KModular/KSynth/VCO.cpp KModular/KSynth/WhiteNoiseOsc.cpp KModular/KSynth/VCF.cpp KModular/KSynth/VCA.cpp KModular/KSynth/moogladder.cpp KModular/KEffect/ReverbModule.cpp KModular/Keffect/DelayModule.cpp KModular/Keffect/ChorusModule.cpp KModular/Keffect/Repeater.cpp KModular/Keffect/delay/AudioBuffer.cpp KModular/Keffect/delay/CircularBuffer.cpp KModular/Keffect/delay/Delay.cpp KModular/Keffect/delay/MultiTapDelay.cpp

# Library Locations
LIBDAISY_DIR = ../../libDaisy/
DAISYSP_DIR = ../../DaisySP/

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

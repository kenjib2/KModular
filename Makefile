# Project Name
TARGET = KenjiSynth

# Sources
CPP_SOURCES = KiwiModular.cpp KModular/KSynth/KSynth.cpp KModular/KSynth/Voice.cpp KModular/KSynth/VCO.cpp KModular/KSynth/WhiteNoiseOsc.cpp KModular/KSynth/VCF.cpp KModular/KSynth/VCA.cpp KModular/KEffect/ReverbModule.cpp KModular/Keffect/DelayModule.cpp KModular/Keffect/ChorusModule.cpp KModular/MidiTrigger.cpp

# Library Locations
LIBDAISY_DIR = ../../libDaisy/
DAISYSP_DIR = ../../DaisySP/

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

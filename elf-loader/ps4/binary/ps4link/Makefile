ifndef Ps4Sdk
ifdef ps4sdk
Ps4Sdk := $(ps4sdk)
endif
ifdef PS4SDK
Ps4Sdk := $(PS4SDK)
endif
ifndef Ps4Sdk
$(error Neither PS4SDK, Ps4Sdk nor ps4sdk set)
endif
endif



Libraries := -ldebugnet -lps4link -lelfloader -lSceNet_stub

target := ps4_bin
TargetFile := ldr

include $(Ps4Sdk)/make/ps4sdk.mk

all::
	../../../generate/bin/generate Elf $(OutPath)/ldr ../../../local/ldr/ldr.js

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



Libraries := -lelfloader

target := ps4_bin

include $(Ps4Sdk)/make/ps4sdk.mk

all::
	../../../generate/bin/generate Elf $(OutPath)/user ../../../local/ldr/ldr.js

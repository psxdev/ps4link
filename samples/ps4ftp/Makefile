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

target ?= ps4_elf
TargetFile=ps4ftp.elf

include $(Ps4Sdk)/make/ps4sdk.mk
LinkerFlags+=-ldebugnet -lSceNet_stub


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

target ?= ps4_lib_no_all

include $(Ps4Sdk)/make/ps4sdk.mk


$(OutPath)/$(TargetFile).a: $(ObjectFiles)
	$(dirp)
	$(archive)
all:: $(OutPath)/$(TargetFile).a

install:
	@cp $(OutPath)/$(TargetFile).a $(Ps4Sdk)/lib
	@cp include/debugnet.h $(Ps4Sdk)/include
	@echo "Installed!"

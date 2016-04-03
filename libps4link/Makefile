ifdef libps4
LibPS4 := $(libps4)
endif
ifdef LIBPS4
LibPS4 := $(LIBPS4)
endif
ifndef LibPS4
$(error Neither LIBPS4, LibPS4 nor libps4 set)
endif

target ?= ps4-lib

include $(LibPS4)/make/libps4.mk

$(OutPath)/$(TargetFile).a: $(ObjectFiles)
	$(dirp)
	$(archive)

install:
	@cp $(OutPath)/$(TargetFile).a $(libps4)/lib
	@cp include/ps4link.h $(libps4)/include
	@echo "Installed!"

ifdef libps4
LibPS4 := $(libps4)
endif
ifdef LIBPS4
LibPS4 := $(LIBPS4)
endif
ifndef LibPS4
$(error Neither LIBPS4, LibPS4 nor libps4 set)
endif

target ?= ps4-elf
libraries= -ldebugnet
targetfile= ps4ftp.elf
include $(LibPS4)/make/libps4.mk



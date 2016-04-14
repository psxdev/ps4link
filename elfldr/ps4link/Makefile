###################################

ifdef libps4
LibPS4 := $(libps4)
endif
ifdef LIBPS4
LibPS4 := $(LIBPS4)
endif
ifndef LibPS4
$(error Neither LIBPS4, LibPS4 nor libps4 set)
endif

target ?= ps4-bin

ldr ?= elf
ldrgen ?= true

TargetFile := ldr

ifeq ($(target), ps4-lib)
	TargetFile := libps4link
	SourceFilesCForce := source/ps4link.c source/request.c source/commands.c source/debugnet.c 
	Libraries :=  -lps4
	ldrgen := false
endif

include $(LibPS4)/make/libps4.mk

###################################

ifeq ($(ldr), bin)
	CompilerFlags += -DBinaryLoader
endif

ifeq ($(server), true)
	CompilerFlags += -DElfLoaderServer
endif

ifeq ($(memory), emulate)
	CompilerFlags += -DElfLoaderEmulatePS4Memory
endif

ifeq ($(threading), true)
	CompilerFlags += -DElfLoaderThreading
endif

ifeq ($(stdio), none)
	CompilerFlags += -DElfLoaderStandardIORedirectNone
endif
ifeq ($(stdio), lazy)
	CompilerFlags += -DElfLoaderStandardIORedirectLazy
endif
ifeq ($(stdio), wait)
	CompilerFlags += -DElfLoaderStandardIORedirectWait
endif

ifeq ($(target), x86-64)
	Libraries += -lrt -lpthread
endif

###################################

compileAndLinkLdrGen = $(Compiler) $? -std=c11 -O3 -Wall -pedantic -Iinclude -o $@

###################################

Payload ?= 0x926400000
# Binary loader mem layout.
# text#[data#d-payload-area]
# ^     ^   #paycode#paydata
# |-----|-------x      x
#       |--------------|
# allows binldr to use data

###################################

CompilerFlags += -DPayload=$(Payload)

###################################

$(OutPath)/$(TargetFile).a: $(ObjectFiles)
	$(dirp)
	$(archive)

.PHONY:: ldrgen

$(OutPath)/ldrgen: ldrgen/main.c ldrgen/util.c
ifeq ($(target), ps4-bin)
ifneq ($(ldrgen), false)
	$(dirp)
	$(compileAndLinkLdrGen)
ifeq ($(ldr), elf)
	$(OutPath)/ldrgen Elf $(OutPath)/ldr ../local/ldr/ldr.js
else
	$(OutPath)/ldrgen Bin $(OutPath)/ldr ../local/ldr/ldr.js
endif
endif
endif

ldrgen: $(OutPath)/ldrgen

all:: $(OutPath)/ldrgen

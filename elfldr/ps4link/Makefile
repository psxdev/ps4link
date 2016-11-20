###################################

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

target ?= ps4_bin

ldr ?= elf
ldrgen ?= true

TargetFile := ldr

include $(Ps4Sdk)/make/ps4sdk.mk
LinkerFlags+=-lSceNet_stub

###################################

ifeq ($(ldr), bin)
	CompilerFlags += -DBinaryLoader 
endif


###################################

compileAndLinkLdrGen = gcc $? -std=c11 -O3 -Wall -pedantic -Iinclude -o $@

###################################


.PHONY:: ldrgen

$(OutPath)/ldrgen: ldrgen/main.c ldrgen/util.c
ifeq ($(target), ps4_bin)
ifneq ($(ldrgen), false)
	$(dirp)
	$(compileAndLinkLdrGen)
	$(OutPath)/ldrgen Bin $(OutPath)/ldr ../local/ldr/ldr.js
endif
endif

ldrgen: $(OutPath)/ldrgen

all:: $(OutPath)/ldrgen

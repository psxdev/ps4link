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



ldrgen ?= true
OutPath ?= bin


compileAndLinkLdrGen = gcc $? -std=c11 -O3 -Wall -pedantic -Iinclude -o $@

###################################


.PHONY:: generate

$(OutPath)/generate: source/main.c 
ifneq ($(ldrgen), false)
	$(dirp)
	mkdir -p bin
	$(compileAndLinkLdrGen)
endif

generate: $(OutPath)/generate

all:: $(OutPath)/generate

clean: 
	@rm -fR bin 

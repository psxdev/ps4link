#
# Copyright (c) 2015 Antonio Jose Ramos Marquez aka bigboss (@psxdev)
#


ifdef libps4
LibPS4 := $(libps4)
endif
ifdef LIBPS4
LibPS4 := $(LIBPS4)
endif
ifndef LibPS4
$(error Neither LIBPS4, LibPS4 nor libps4 set)
endif


CC	:= clang
AS	:= clang
OBJCOPY	:= objcopy
ODIR	:= build
SDIR	:= source
BDIR	:= bin
IDIRS	:= -I$(LibPS4)/include -I. -Iinclude
LDIRS	:= -L$(LibPS4)/lib 
LIBS	:= -ldebugnet -lps4link -lps4
CFLAGS	:=	-std=c11 -O3 -Wall -pedantic -m64 -mcmodel=large -ffreestanding -nostdlib -nostdinc -fno-builtin -D__PS4__ $(IDIRS)  -target x86_64-scei-ps4-elf  -fPIE
SFLAGS	:=	$(IDIRS) -m64 -fPIE -target x86_64-scei-ps4-elf
LFLAGS	:=	-O3 -Wall -m64 -nostdlib $(LDIRS) -pie $(LIBS)  
CFILES	= $(wildcard $(SDIR)/*.c)
SFILES	= $(wildcard $(SDIR)/*.s)
OBJS	= $(ODIR)/main.o
TARGET = $(shell basename $(CURDIR))

ALL: $(ODIR) $(BDIR) $(TARGET)

$(TARGET):  $(OBJS)
	$(CC) $(LibPS4)/crt0.s $^  $(LFLAGS)  -o $(BDIR)/$@.elf
	$(OBJCOPY) $(BDIR)/$@.elf -O binary $(BDIR)/$@.bin

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c $< $(CFLAGS) -o $@

$(ODIR):
	@mkdir $@

$(BDIR):
	@mkdir $@

.PHONY: clean

clean:
	rm -f $(BDIR)/*.elf $(BDIR)/*.bin $(ODIR)/*.o




TARGET_LIB = libdebugnet.a
OBJS       = source/debugnet.o 
INCLUDES   = ./include -I$(libps4)/include
CC      = clang
AR      = ar
CFLAGS  =  -std=c11 -O3 -Wall -pedantic -m64 -mcmodel=large -ffreestanding -nostdlib -nostdinc -fno-builtin -D__PS4__ -I$(INCLUDES) -target x86_64-scei-ps4-elf -D"LibPS4SyscallDirect" -fPIC   
ASFLAGS = $(CFLAGS)

all: $(TARGET_LIB)


$(TARGET_LIB): $(OBJS)
	$(AR) -rcs $@ $^ 

clean:
	@rm -rf $(TARGET_LIB) $(OBJS)

install: 
	@cp $(TARGET_LIB) $(libps4)/lib
	@cp include/debugnet.h $(libps4)/include
	@echo "Installed!"

#pragma once

#include <ps4/memory.h>
#include <ps4/error.h>

#include <sys/sysent.h>

/* Types */

typedef int (*ElfMain)(int argc, char **argv);
typedef void (*ElfProcessMain)(void *arg);

typedef struct ElfRunUserArgument
{
	ElfMain main;
	Ps4MemoryProtected *memory;
	//Ps4Memory *memory;
}
ElfRunUserArgument;

typedef struct ElfRunKernelArgument
{
	int isProcess;
	size_t size;
	void *data;
}
ElfRunKernelArgument;

int elfLoaderKernMain(struct thread *td, void *uap);

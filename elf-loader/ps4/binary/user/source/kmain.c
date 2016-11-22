#define _KERNEL

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>

#include <ps4/kernel.h>

#include <elfloader.h>

#include "main.h"

typedef struct ElfKernelProcessInformation
{
	struct proc *process;
	uint8_t *processMain;
	uint8_t *main;
	int argc;
	char *argv[3];
	char elfName[128];
	void *processFree;
	void *processMemoryType;
	void *processExit;
}
ElfKernelProcessInformation;

typedef void (*ElfProcessExit)(int ret);
typedef void (*ElfProcessFree)(void *m, void *t);

// copy-able
void elfPayloadProcessMain(void *arg)
{
	ElfKernelProcessInformation *pargs = arg;
	//ElfProcessExit pexit = (ElfProcessExit)pargs->processExit;
	((ElfMain)pargs->main)(pargs->argc, pargs->argv);
	((ElfProcessFree)pargs->processFree)(pargs, pargs->processMemoryType);
	//pexit(0); //FIXME: Hmm? Oo -> panics, should not, example sys/dev/mmc/mmcsd.c
}

int elfLoaderKernMain(struct thread *td, void *uap)
{
	ElfRunKernelArgument *arg;
	ElfKernelProcessInformation *pargs;
	char buf[32];
	Elf *elf;
	int isProcess, r;
	int elfPayloadProcessMainSize = 64; // adapt if needed

	arg = (ElfRunKernelArgument *)uap;
	if(arg == NULL || arg->data == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	isProcess = arg->isProcess;
 	elf = elfCreateLocalUnchecked((void *)buf, arg->data, arg->size);

	pargs = ps4KernelMemoryMalloc(sizeof(ElfKernelProcessInformation) + elfPayloadProcessMainSize + elfMemorySize(elf));
	if(pargs == NULL)
	{
		ps4KernelMemoryFree(arg->data);
		ps4KernelMemoryFree(arg);
		return PS4_ERROR_KERNEL_OUT_OF_MEMORY;
	}

	// memory = (info, procmain, main)
	pargs->processMain = (uint8_t *)pargs + sizeof(ElfKernelProcessInformation);
	pargs->main = pargs->processMain + elfPayloadProcessMainSize;
	r = elfLoaderLoad(elf, pargs->main, pargs->main); // delay error return til cleanup
	pargs->main += elfEntry(elf);

	// aux
	pargs->argc = 1;
	pargs->argv[0] = pargs->elfName;
	pargs->argv[1] = NULL;
	pargs->argv[2] = NULL;
	sprintf(pargs->elfName, "ps4sdk-elf-%p", pargs);

	// Free user argument
	ps4KernelMemoryFree(arg->data);
	ps4KernelMemoryFree(arg);

	if(r != ELF_LOADER_RETURN_OK)
	{
		ps4KernelMemoryFree(pargs);
		return r;
	}

	if(!isProcess)
	{
		int r;
		r = ((ElfMain)pargs->main)(pargs->argc, pargs->argv);
		ps4KernelMemoryFree(pargs);
		ps4KernelThreadSetReturn(td, (register_t)r);
		return PS4_OK;
	}

	ps4KernelSymbolLookUp("free", &pargs->processFree);
	ps4KernelSymbolLookUp("M_TEMP", &pargs->processMemoryType);
	ps4KernelSymbolLookUp("kproc_exit", &pargs->processExit);
	ps4KernelMemoryCopy((void *)elfPayloadProcessMain, pargs->processMain, elfPayloadProcessMainSize);

	if(kproc_create((ElfProcessMain)pargs->processMain, pargs, &pargs->process, 0, 0, "ps4sdk-elf-%p", pargs) != 0)
	{
		ps4KernelMemoryFree(pargs);
		return PS4_ERROR_KERNEL_PROCESS_NOT_CREATED;
	}

	ps4KernelThreadSetReturn(td, (register_t)pargs->process); // FIXME: Races against free
	return PS4_OK; //FIXME: This does not return 0 Oo?
}

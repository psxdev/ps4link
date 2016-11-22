#include <kernel.h>
#include <debugnet.h>
#include <net.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/syscall.h>

//
//Thanks http://code.taobao.org/p/CryEngine3_6_3/
//

typedef struct SceKernelVirtualQueryInfo {
    void* start;
    void* end;
    off_t offset;
    int protection;
    int unk0;
    unsigned unk1:1;
    unsigned isDirectMemory:1;
    unsigned unk3:1;
    char name[32];
} SceKernelVirtualQueryInfo;

static int GetVirtualMemInfo() {
	int ptrwidth;         
	ptrwidth = 2*sizeof(void *) + 2;
	SceKernelVirtualQueryInfo info;
	void *addr=NULL;
	size_t size=0;
	debugNetUDPPrintf("%-32s %s\t%s\t\t%s\n", "NAME", "START", "END", "PROT");
	//sceKernelVirtualQuery(addr, SCE_KERNEL_VQ_FIND_NEXT, &info, sizeof(info))==SCE_OK)
	while (syscall(572, addr, 1, &info, sizeof(info))==0)
	{
		if (!info.isDirectMemory) {
			size+=(int*)info.end-(int*)info.start;
		}

		addr=info.end;

		debugNetUDPPrintf("%-32s 0x%08x\t0x%08x\t0x%02x\n", info.name, info.start, info.end, info.protection);
	}
	return size;
}

int main(int argc, char *argv[])
{
	int ret;
	struct kinfo_proc *p;
	uintptr_t intptr=0;
	sscanf(argv[1],"%p",&intptr);
	debugNetConfiguration *conf=(debugNetConfiguration *)intptr;
	ret=debugNetInitWithConf(conf);
	sleep(3);
	debugNetPrintf(DEBUG,"[VirtualMemQuery] %d elfname=%s ps4linkconf=%s %p %d\n",argc,argv[0],argv[1],conf,conf->SocketFD);

	debugNetUDPPrintf("Current Executable Size: %d\n", GetVirtualMemInfo());

	debugNetFinish();
	
	return EXIT_SUCCESS;
}

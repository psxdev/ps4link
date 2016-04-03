/*
 * PS4link library for PlayStation 4 to communicate and use host file system with ps4sh host tool 
 * Copyright (C) 2003,2015,2016 Antonio Jose Ramos Marquez (aka bigboss) @psxdev on twitter
 * Repository https://github.com/psxdev/ps4link
 * based on ps2vfs, ps2client, ps2link, ps2http tools. 
 * Credits goes for all people involved in ps2dev project https://github.com/ps2dev
 * This file is subject to the terms and conditions of the PS4Link License.
 * See the file LICENSE in the main directory of this distribution for more
 * details.
 */
#include <debugnet.h>
#include <net.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <ps4/protectedmemory.h>
#include <ps4/elfloader.h>
#include <inttypes.h>
#include "ps4link.h"
#include "ps4link_internal.h"


extern int ftp_initialized;
typedef int (*Runnable)(int, char **);
typedef struct MainAndMemory
{
	Runnable main;
	PS4ProtectedMemory *memory;
}MainAndMemory;
MainAndMemory mm;

ScePthread elf_thid;

int GID=-1;
int UID=-1;

#define BUF_SIZE 1024
static char recvbuf[BUF_SIZE] __attribute__((aligned(16)));
extern ps4LinkConfiguration *configuration;

void *ps4LinkRunElfMain()
{
	//MainAndMemory *mm;
	int ret;
	ps4LinkConfiguration *shared_conf=configuration;
	char pointer_conf[256];
	sprintf(pointer_conf,"%p",shared_conf->debugconf);
	debugNetPrintf(DEBUG,"[PS4LINK] Configuration pointer %p, pointer_conf string %s\n",shared_conf->debugconf,pointer_conf);
	char *elfName = "elf";
	char *elfArgv[2] = { elfName, pointer_conf };
	int elfArgc = 2;
	
	if(&mm == NULL)
	{
		debugNetPrintf(DEBUG,"[PS4LINK] mainAndMemory is NULL\n");
		return NULL;
	}
	debugNetPrintf(DEBUG,"[PS4LINK] ps4LinkRunElfMain\n");
	
	//mm = (MainAndMemory *)mainAndMemory;
	ret = mm.main(elfArgc, elfArgv);
	debugNetPrintf(DEBUG,"[PS4LINK] ps4LinkRunElfMain mm->main return %i\n", ret);
	ps4ProtectedMemoryDestroy(mm.memory);	
	//free(mm);
	return NULL;	
}
void ps4LinkRunElf(Elf *elf)
{

	size_t size;
	int ret;
	
	//MainAndMemory *mm;
	
	//sanity check however we only call this functionn with a valid elf from ps4LinkCmdExecElf
	if(!elf)
	{
		return;
	}

	//mm = (MainAndMemory *)malloc(sizeof(MainAndMemory));
 	//mm = (MainAndMemory *)malloc(sizeof(MainAndMemory));
	
	if(&mm ==  NULL)
	{
		debugNetPrintf(DEBUG,"[PS4LINK] MainAndMemory allocation failed\n");
		debugNetPrintf(DEBUG,"[PS4LINK] elfDestroyAndFree(%p)\n", (void *)elf);
		elfDestroyAndFree(elf);
		return;
	}
	size = elfMemorySize(elf);
	debugNetPrintf(DEBUG,"ps4ProtectedMemoryCreate(%zu) ->\n ", size);
	//mm->memory=ps4ProtectedMemoryCreate(size);
	mm.memory=ps4ProtectedMemoryCreate(size);
	
	//if(!mm->memory)
	if(!mm.memory)	
	{
		//debugNetPrintf(DEBUG,"[PS4LINK] Elf (%p)  or memory (%p) NULL\n", (void *)elf, (void *)mm->memory);
		debugNetPrintf(DEBUG,"[PS4LINK] Elf (%p)  or memory (%p) NULL\n", (void *)elf, (void *)mm.memory);
		
		return;
	}
	//debugNetPrintf(DEBUG,"[PS4LINK] elfLoaderLoad(%p, %p, %p) -> \n", (void *)elf, ps4ProtectedMemoryWritable(mm->memory), ps4ProtectedMemoryExecutable(mm->memory));
	debugNetPrintf(DEBUG,"[PS4LINK] elfLoaderLoad(%p, %p, %p) -> \n", (void *)elf, ps4ProtectedMemoryWritable(mm.memory), ps4ProtectedMemoryExecutable(mm.memory));
	
	//ret = elfLoaderLoad(elf, ps4ProtectedMemoryWritable(mm->memory), ps4ProtectedMemoryExecutable(mm->memory));
	ret = elfLoaderLoad(elf, ps4ProtectedMemoryWritable(mm.memory), ps4ProtectedMemoryExecutable(mm.memory));
	
	if(ret<0)
	{
		debugNetPrintf(DEBUG,"[PS4LINK] Elf could not be loaded error code %i\n", ret);
	//	elfDestroyAndFree(elf); // we don't need the "file" anymore
		//free(mm);
		return;
	}
	else
	{
		debugNetPrintf(DEBUG,"[PS4LINK] elfLoaderLoad return %i\n", ret);
	//	mm->main = (Runnable)((uint8_t *)ps4ProtectedMemoryExecutable(mm->memory) + elfEntry(elf));
		mm.main = (Runnable)((uint8_t *)ps4ProtectedMemoryExecutable(mm.memory) + elfEntry(elf));
		
	//	debugNetPrintf(DEBUG,"[PS4LINK] mm->main %p \n", mm->main);
		debugNetPrintf(DEBUG,"[PS4LINK] mm->main %p \n", mm.main);
		
		//elfDestroyAndFree(elf); // we don't need the "file" anymore
		
		
	}
	//if(mm->main != NULL)
	
	if(mm.main != NULL)
	{
	//	debugNetPrintf(DEBUG,"PS4LINK run [%p + elfEntry = %p]\n", ps4ProtectedMemoryExecutable(mm->memory), (void *)mm->main);
		debugNetPrintf(DEBUG,"PS4LINK run [%p + elfEntry = %p]\n", ps4ProtectedMemoryExecutable(mm.memory), (void *)mm.main);
		//ret=scePthreadCreate(&elf_thid, NULL, ps4LinkRunElfMain, mm, "elf_thid");
		
		ret=scePthreadCreate(&elf_thid, NULL, ps4LinkRunElfMain, &mm, "elf_thid");
		if(ret==0)
		{
			debugNetPrintf(DEBUG,"[PS4LINK] New elf thread UID: 0x%08X\n", elf_thid);			
		}
		else
		{
			debugNetPrintf(DEBUG,"[PS4LINK] New elf thread could not create error: 0x%08X\n", ret);
			scePthreadCancel(elf_thid);
			//ps4LinkFinish();
			return;
		}
	}
	else
	{
		//free(mm);
	}
}
Elf * ps4LinkReadElfFromHost(char *path)
{
	int fd; //descriptor to manage file from host0
	int filesize;//variable to control file size 
	uint8_t *buf=NULL;//buffer for read from host0 file
	Elf *elf;//elf to create from buf 
	
	//we open file in read only from host0 ps4sh include the full path with host0:/.......
	fd=ps4LinkOpen(path,O_RDONLY,0);

	//If we can't open file from host0 print  the error and return
	if(fd<0)
	{
		debugNetPrintf(DEBUG,"[PS4LINK] ps4LinkOpen returned error %d\n",fd);
		return NULL;
	}
	//Seek to final to get file size
	filesize=ps4LinkLseek(fd,0,SEEK_END);
	//If we get an error print it and return
	if(filesize<0)
	{
		debugNetPrintf(DEBUG,"[PS4LINK] ps4LinkSeek returned error %d\n",fd);
		ps4LinkClose(fd);
		return NULL;
	}
	//Seek back to start
	ps4LinkLseek(fd,0,SEEK_SET);
	//Reserve  memory for read buffer
	//buf=malloc(filesize);
	//char buf[filesize];
	buf=mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	
	
	//Read filsesize bytes to buf
	int numread=ps4LinkRead(fd,buf,filesize);
	//if we don't get filesize bytes we are in trouble
	if(numread!=filesize)
	{
		debugNetPrintf(DEBUG,"[PS4LINK] ps4LinkRead returned error %d\n",numread);
		ps4LinkClose(fd);
		return NULL;		
	}
	//Close file
	ps4LinkClose(fd);
	//create elf from elfloader code from hitodama :P
	elf = elfCreate((void*)buf, filesize);
	//check is it is loadable
	if(!elfLoaderIsLoadable(elf))
	{
		debugNetPrintf(DEBUG,"[PS4LINK] elf %s is not loadable\n",path);
		//free(buf);
		elfDestroy(elf);
		elf = NULL;
		
	}
	
	
	return elf;

	
}
void ps4LinkCmdExecElf(ps4link_pkt_exec_cmd *pkg)
{

	Elf *elf=NULL;
	debugNetPrintf(DEBUG,"[PS4LINK] Received command execelf argc=%x argv=%s\n",ntohl(pkg->argc),pkg->argv);
	elf=ps4LinkReadElfFromHost(pkg->argv);
	if(elf==NULL)
	{
		debugNetPrintf(DEBUG,"[PS4LINK] we can't create elf\n");
	}
	debugNetPrintf(DEBUG,"[PS4LINK] ready to run elf\n");
	
	ps4LinkRunElf(elf);
	return;
}
char *entryName(int entryType) {
	switch(entryType) {
		case DT_DIR: return "DIR";
		case DT_REG: return "FILE";
		default: return "OTHER";
	}
}
void ps4LinkCmdExecSprx(ps4link_pkt_exec_cmd *pkg)
{
	debugNetPrintf(DEBUG,"[PS4LINK] Received command execsprx argc=%d argv=%s\n",ntohl(pkg->argc),pkg->argv);
	
	
}
void ps4LinkCmdExecWhoami(ps4link_pkt_exec_cmd *pkg)
{
	debugNetPrintf(DEBUG,"[PS4LINK] Received command execwhoami\n");
	UID=getuid();
	GID=getgid();
	
	debugNetPrintf(DEBUG,"[PS4LINK] UID: %d, GID: %d\n",UID,GID);
		
}
void ps4LinkCmdExecShowDir(ps4link_pkt_exec_cmd *pkg)
{
	debugNetPrintf(DEBUG,"[PS4LINK] Received command execshowdir\n");
	char *buffer;
	struct dirent *dent;
	struct stat stats;
	int dfd;
	int i;
	if(UID==0 && GID==0 && pkg->argv!=NULL)
	{
		
		dfd = open(pkg->argv, O_RDONLY, 0);
		if(dfd < 0) {
			debugNetPrintf(DEBUG, "Invalid directory.\n");
			return;
		}
		int err=fstat(dfd, &stats);
		if(err<0)
		{
			debugNetPrintf(DEBUG, "fstat error return  0x%08X \n",err);
			return;
		}
		buffer=mmap(NULL, stats.st_blksize+sizeof(struct dirent), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
		if (buffer)
		{
			// Make sure we will have a null terminated entry at the end.Thanks people leaving CryEngine code for orbis on github  :)
			for(i=0;i<stats.st_blksize+sizeof(struct dirent);i++)
			{
				buffer[i]=0;
			}
			err=getdents(dfd, buffer, stats.st_blksize);

			int nOffset = err;
			while (err > 0 && err < stats.st_blksize)
			{
				err = getdents(dfd, buffer + nOffset, stats.st_blksize-nOffset);
				nOffset += err;
			}
			
			if (err>0)
				err=0;
			
			dent = (struct dirent *)buffer;
			while(dent->d_fileno ) {
					debugNetPrintf(DEBUG, "[%s]: %s\n", entryName(dent->d_type), dent->d_name);

					dent = (struct dirent *)((void *)dent + dent->d_reclen);
			}
		}
		munmap(buffer,stats.st_blksize+sizeof(struct dirent));
		
		
		
		

		debugNetPrintf(DEBUG,"[PS4LINK] closing dfd\n");

		err=close(dfd);  
		if(err<0)
		{
				debugNetPrintf(DEBUG, "fstat error return  0x%08X \n",err);
					return;
		}
	}
	else
	{
		if(pkg->argv!=NULL)
		{
			debugNetPrintf(DEBUG,"Sorry you are not root , you must be root to run this...\n");
		}
		else
		{
			debugNetPrintf(DEBUG,"Sorry you must provide a ps4 directory path...\n");
			
		}
	}
	debugNetPrintf(DEBUG,"[PS4LINK] end command execshowdir\n");
	
}
void ps4LinkCmdExit(ps4link_pkt_exec_cmd *pkg)
{
	debugNetPrintf(DEBUG,"[PS4LINK] Received command exit. Closing PS4Link...\n");
	
	ps4LinkFinish();
	
}
int ps4link_commands_thread(void* args)
{
	struct sockaddr_in serveraddr;
	//    struct sockaddr_in remote_addr;
	int ret;
	int len;
	int addrlen;
	unsigned int cmd;
	ps4link_pkt_hdr *header;
	struct sockaddr_in remote_addr;
	
	
	
	
	debugNetPrintf(DEBUG,"[PS4LINK] Command Thread Started.\n" );
	
	configuration->ps4link_commands_sock = sceNetSocket("commands_server_sock",AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	if (ps4LinkGetValue(COMMANDS_SOCK) >=0)
	{
		debugNetPrintf(DEBUG,"[PS4LINK] Created ps4link_commands_sock: %d\n", ps4LinkGetValue(COMMANDS_SOCK));
	}
	else
	{
		debugNetPrintf(DEBUG,"[PS4LINK] Error creating socket ps4link_commands_sock  %d\n", ps4LinkGetValue(COMMANDS_SOCK));
		ps4LinkFinish();
		return -1;
	}
	/* Fill the server's address */
    memset(&serveraddr, 0, sizeof serveraddr);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = sceNetHtonl(INADDR_ANY);
	serveraddr.sin_port = sceNetHtons(ps4LinkGetValue(COMMANDS_PORT));
	memset(&(serveraddr.sin_zero), '\0', 8);
	
	
   
	ret = sceNetBind(ps4LinkGetValue(COMMANDS_SOCK), (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	
	
	
	
	if (ret < 0) 
	{
		debugNetPrintf(DEBUG,"[PS4LINK] command listener sceNetBind error: 0x%08X\n", ret);
		sceNetSocketClose(ps4LinkGetValue(COMMANDS_SOCK));
		ps4LinkFinish();
		return -1;
	}
	// Do tha thing
	debugNetPrintf(DEBUG,"[PS4LINK] Command listener waiting for commands...\n");
	
	
	
	
	
	
	


	while(ps4LinkGetValue(CMDSIO_ACTIVE)) {

		addrlen = sizeof(remote_addr);
		//wait for new command
		
		len = recvfrom(ps4LinkGetValue(COMMANDS_SOCK), &recvbuf[0], BUF_SIZE, 0, (struct sockaddr *)&remote_addr,&addrlen);
		
		debugNetPrintf(DEBUG,"[PS4LINK] commands listener received packet size (%d)\n", len);

		if (len < 0) {
			debugNetPrintf(DEBUG,"[PS4LINK] commands listener recvfrom size error (%d)\n", len);
			continue;
		}
		if (len < sizeof(ps4link_pkt_hdr)) {
			debugNetPrintf(DEBUG,"[PS4LINK] commands listener recvfrom header size error (%d)\n", len);
			continue;
		}

		header = (ps4link_pkt_hdr *)recvbuf;
		cmd = ntohl(header->cmd);
		
		switch (cmd) {

			case PS4LINK_EXECELF_CMD:
				ps4LinkCmdExecElf((ps4link_pkt_exec_cmd *)recvbuf);
				break;
			case PS4LINK_EXECSPRX_CMD:
				ps4LinkCmdExecSprx((ps4link_pkt_exec_cmd *)recvbuf);
				break;
			case PS4LINK_EXIT_CMD:
				ps4LinkCmdExit((ps4link_pkt_exec_cmd *)recvbuf);
				break;
			case PS4LINK_EXECWHOAMI_CMD:
				ps4LinkCmdExecWhoami((ps4link_pkt_exec_cmd *)recvbuf);
				break;
			case PS4LINK_EXECSHOWDIR_CMD:
				ps4LinkCmdExecShowDir((ps4link_pkt_exec_cmd *)recvbuf);
				break;
			default: 
				debugNetPrintf(DEBUG,"[PS4LINK] Unknown command received\n");
				break;
		}
		debugNetPrintf(DEBUG,"[PS4LINK] commands listener waiting for next command\n");
	}
	debugNetPrintf(DEBUG,"[PS4LINK] exit commands listener thread\n");
	if(ps4LinkGetValue(COMMANDS_SOCK))
	{
		debugNetPrintf(DEBUG,"[PS4LINK] closing server_commands_sock\n");
		sceNetSocketClose(ps4LinkGetValue(COMMANDS_SOCK));
		configuration->ps4link_commands_sock=-1;
	}
	
	//sceKernelExitDeleteThread(0);
	return 0;
}

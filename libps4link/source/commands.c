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


#include "ps4link.h"
#include "ps4link_internal.h"


#define BUF_SIZE 1024
static char recvbuf[BUF_SIZE] __attribute__((aligned(16)));
extern ps4LinkConfiguration *configuration;
void ps4LinkCmdExecElf(ps4link_pkt_exec_cmd *pkg)
{
	debugNetPrintf(DEBUG,"[PS4LINK] Received command execelf argc=%d argv=%s\n",ntohl(pkg->argc),pkg->argv);
	int fd=ps4LinkOpen(pkg->argv,O_RDONLY,0);
	if(fd<0)
	{
		debugNetPrintf(DEBUG,"[PS4LINK] ps4LinkOpen returned error %d\n",fd);
		return;
	}
	int filesize=ps4LinkLseek(fd,0,SEEK_END);
	if(filesize<0)
	{
		debugNetPrintf(DEBUG,"[PS4LINK] ps4LinkSeek returned error %d\n",fd);
		return;
	}
	ps4LinkLseek(fd,0,SEEK_SET);
	char buf[filesize+1];
	int numread=ps4LinkRead(fd,buf,filesize);
	if(numread!=filesize)
	{
		debugNetPrintf(DEBUG,"[PS4LINK] ps4LinkRead returned error %d\n",numread);
		ps4LinkClose(fd);
		return;		
	}
	debugNetPrintf(DEBUG,"[PS4LINK] Content of file: %s \n",buf);
	ps4LinkClose(fd);
	
}
void ps4LinkCmdExecSprx(ps4link_pkt_exec_cmd *pkg)
{
	debugNetPrintf(DEBUG,"[PS4LINK] Received command execsprx argc=%d argv=%s\n",pkg->argc,pkg->argv);
	
	
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

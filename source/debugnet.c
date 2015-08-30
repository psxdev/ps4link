/*
 * ps4link library for PS4 to communicate and use host file system with ps4client host tool 
 * Copyright (C) 2003,2015 Antonio Jose Ramos Marquez (aka bigboss) @psxdev on twitter
 * Repository https://github.com/psxdev/ps4link
 * based on psp2link, ps2vfs, ps2client, ps2link, ps2http tools. 
 * Credits goes for all people involved in ps2dev project https://github.com/ps2dev
 * This file is subject to the terms and conditions of the ps4link License.
 * See the file LICENSE in the main directory of this distribution for more
 * details.
 */
#include "ps4.h"
#include "debugnet.h"

int debugNetInit(char *serverIp,int port)
{
	int ret;
	
	
	// Connect to server and send message
	char socketName[] = "debug";

	struct sockaddr_in server;

	//udp log to port 18194
	server.sin_len = 16;
	server.sin_family = 2;
	sceNetInetPton(2, serverIp, &server.sin_addr);
	server.sin_port = sceNetHtons(port);
	memset(server.sin_zero, 0, sizeof(server.sin_zero));

	debugnet_sock = sceNetSocket(socketName, 2, 2, 0);
	if(debugnet_sock<0)
	{
		return debugnet_sock;
	}
	ret=sceNetConnect(debugnet_sock, (struct sockaddr *)&server, sizeof(server));
	if(ret!=0)
	{
		return ret;
	}

	DEBUG("debugnet Initialized\n");
	DEBUG("debugnet jaaaar\n");
	
	
	
	return 1;
	
}
void debugNetFinish()
{
	sceNetSocketClose(debugnet_sock);
}

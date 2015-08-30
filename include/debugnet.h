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

int debugnet_sock;
#define debug(...) \
	do { \
		char debug_buffer[512]; \
		int tam = sprintf(debug_buffer, ##__VA_ARGS__); \
		sceNetSend(debugnet_sock, debug_buffer, tam, 0); \
	} while(0)


#define DEBUG(...) debug(__VA_ARGS__)
#define INFO(...) debug(__VA_ARGS__)

int debugNetInit(char *serverIp, int port);
void debugNetFinish();

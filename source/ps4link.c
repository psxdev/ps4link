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
#include "ps4link.h"

void *ps4link_requests_thread(void *arg);
void *ps4link_commands_thread(void *arg);

int  ps4link_initialized;
int  ps4link_requests_port;
int  ps4link_commands_port;
int  ps4link_fileio_active; 
ScePthread server_request_thid;
ScePthread server_command_thid;



/**
 * Init ps4link library 
 *
 * @par Example:
 * @code
 * int ret;
 * ret = ps4LinkInit("172.26.0.2",0x4711,0x4712,0x4712);
 * @endcode
 * 
 * @param serverIp - server ip for udp debug
 * @param requestPort - ps4  port server for requests
 * @param debugPort - udp port for debug
 * @param commandPort - ps4  port server for commands
 */
int ps4LinkInit(char *serverIp, int requestPort,int debugPort, int commandPort)
{
	int ret;
	ps4link_requests_port=requestPort;
	ps4link_commands_port=commandPort;
	ps4link_fileio_active=1;
	
	
	if(debugNetInit(serverIp,debugPort))
	{
		ret=scePthreadCreate(&server_request_thid, NULL, ps4link_requests_thread, NULL, "ps4link_request_server_thread");
		

		if(ret==0)
		{
			DEBUG("Server request thread UID: 0x%08X\n", server_request_thid);
			
		}
		else
		{
			DEBUG("Server request thread could not create error: 0x%08X\n", ret);
			scePthreadCancel(server_request_thid);
			ps4LinkFinish();
			return 0;
		}
	
		ret=scePthreadCreate(&server_command_thid, NULL, ps4link_commands_thread, NULL, "psp4link_command_server_thread");
		
		

		if(ret==0)
		{
			DEBUG("Server command thread UID: 0x%08X\n", server_command_thid);
			
			
		}
		else
		{
			DEBUG("Server command thread could not create error: 0x%08X\n", server_command_thid);
			scePthreadCancel(server_request_thid);
			scePthreadCancel(server_command_thid);
			ps4LinkFinish();
			return 0;
		}
	
		
	
		/*library psp2link initialized*/	
	    ps4link_initialized = 1;
		
	}
	else
	{
		ps4link_initialized = 0;
	}

    return ps4link_initialized;
}
int ps4LinkGetValue(ps4LinkValue val)
{
	int ret;
	switch(val)
	{
		case FILEIO_ACTIVE:
			ret=ps4link_fileio_active;
			break;
		case REQUESTS_PORT:
			ret=ps4link_requests_port;
			break;
		case COMMANDS_PORT:
			ret=ps4link_commands_port;
			break;
		default:
			ret=0;
			break;
	}
	return ret;
}
/**
 * Finish ps4link library 
 *
 * @par Example:
 * @code
 * ps4LinkFinish();
 * @endcode
 */
void ps4LinkFinish()
{
	ps4link_fileio_active=0;
    ps4link_initialized = 0;
	ps4LinkRequestsAbort();
	while(ps4LinkRequestsIsConnected())
	{
		
	}
	debugNetFinish();
	
	
}

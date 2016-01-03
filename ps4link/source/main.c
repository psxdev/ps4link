/*
 * PS4link loader for PlayStation 4 to communicate and use host file system with ps4sh host tool 
 * Copyright (C) 2003,2015,2016 Antonio Jose Ramos Marquez (aka bigboss) @psxdev on twitter
 * Repository https://github.com/psxdev/ps4link
 * based on ps2vfs, ps2client, ps2link, ps2http tools. 
 * Credits goes for all people involved in ps2dev project https://github.com/ps2dev
 * This file is subject to the terms and conditions of the PS4Link License.
 * See the file LICENSE in the main directory of this distribution for more
 * details.
 */

#include <stdio.h>
#include <fcntl.h>
#include <ps4link.h>
#include <debugnet.h>


int main(void) {
	
	int handle;
	int ret;
	int count;
	int index;
	int modules[256];
	
	
	
	
	ret=ps4LinkInit("192.168.1.3",0x4711,0x4712,0x4712,DEBUG);
	if(!ret)
	{
		ps4LinkFinish();
		return ret;
	}
	while(!ps4LinkRequestsIsConnected())
	{
		
	}
	
	
	debugNetPrintf(DEBUG,"[PS4LINK] Initialized and connected from pc/mac ready to receive commands\n");
	


	// Return to browser threads working in the background
	return 0;
}

/*
 *	debugnet library sample for PlayStation 4 
 *	Copyright (C) 2010,2016 Antonio Jose Ramos Marquez (aka bigboss) @psxdev on twitter
 *  Repository https://github.com/psxdev/ps4link
 */
#include <debugnet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>





int main(int argc, char *argv[])
{
	int ret;
	uintptr_t intptr=0;
	sscanf(argv[1],"%p",&intptr);
	debugNetConfiguration *conf=(debugNetConfiguration *)intptr;
	ret=debugNetInitWithConf(conf);
	sleep(3);
	debugNetPrintf(DEBUG,"[SAMPLE] %d elfname=%s ps4linkconf=%s %p %d\n",argc,argv[0],argv[1],conf,conf->SocketFD);
	debugNetFinish();
	
	return EXIT_SUCCESS;
}

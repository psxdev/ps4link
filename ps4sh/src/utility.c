/*
 * psp2client host tool for PSP2 providing host fileio system 
 * Copyright (C) 2003,2015 Antonio Jose Ramos Marquez (aka bigboss) @psxdev on twitter
 * Repository https://github.com/psxdev/psp2client
 * based on ps2vfs, ps2client, ps2link, ps2http tools. 
 * Credits goes for all people involved in ps2dev project https://github.com/ps2dev
 * This file is subject to the terms and conditions of the PSP2Client License.
 * See the file LICENSE in the main directory of this distribution for more
 * details.
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "utility.h"

///////////////////////
// UTILITY FUNCTIONS //
///////////////////////

int fix_flags(int flags) 
{ 
	int result = 0;


	// Fix the flags.
	if (!(flags & 0x0001)) { result |= O_RDONLY;           }
	if (flags & 0x0001) { result |= O_WRONLY | O_TRUNC; } // FIXME: Truncate is needed for some programs.
	if (flags & 0x0002) { result |= O_RDWR | O_TRUNC; } // FIXME: Truncate is needed for some programs.
	
#ifndef _WIN32
	if (flags & 0x2000) { result |= O_NONBLOCK;         }
#endif
	if (flags & 0x1000) { result |= O_APPEND;           }
	if (flags & 0x0100) { result |= O_CREAT;            }
	if (flags & 0x0800) { result |= O_TRUNC;            }

#ifdef _WIN32

	// Binary mode file access.
	result |= O_BINARY;

#endif

	// End function.
	return result;

}
int fix_pathname(char *pathname) 
{ 
	int loop0 = 0;
	char *token;
	// If empty, set a pathname default.
	if (pathname[0] == 0) { strcpy(pathname, "."); }

	// Convert \ to / for unix compatibility.
	for (loop0=0; loop0<strlen(pathname); loop0++) { if (pathname[loop0] == '\\') { pathname[loop0] = '/'; } }

	char *aux=strdup(pathname);
	
	token=strtok(aux,":");
	
	if(strcmp(token,"host0")==0)
	{
		for(loop0=0; loop0<strlen(pathname)-6; loop0++) { pathname[loop0] = pathname[loop0+6]; }
		pathname[loop0] = 0;
		printf("aqui\n");
		
	}
	else
	{
		printf("Path received does not include host0: %s\n",pathname);
		pathname[0]=0;
	}

// End function.
	return 0;

}

int fix_argv(char *destination, char **argv) 
{ 
	int loop0 = 0;

	// For each argv...
	for (loop0=0; argv[loop0]; loop0++) {

		// Copy the argv to the destination.
		memcpy(destination, argv[loop0], strlen(argv[loop0]));

		// Increment the destination pointer.
		destination += strlen(argv[loop0]);

		// Null-terminate the argv.
		*destination = 0;

		// Increment the destination pointer.
		destination += 1;

	}

	// End function.
	return 0;

}

int print_usage(void) 
{

	// Print out the psp2client usage string.
	printf("\n");
	printf(" Usage: psp2client [-h hostname] [-t timeout] <command> [arguments]\n\n");
	printf(" Available commands:\n\n");
	printf("   reset\n");
	printf("   execpsp2 <filename> [arguments]\n");
	printf("   listen\n\n");

	// End function.
	return 0;
}

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
#pragma once

#include "types.h"
#include "file.h"

typedef enum ps4LinkValue
{
	FILEIO_ACTIVE= 1,
	REQUESTS_PORT= 2,
	COMMANDS_PORT= 3,
	
} ps4LinkValue; 
int ps4LinkOpen(const char *file, int flags, int mode);
int ps4LinkClose(int fd);
int ps4LinkRead(int fd, void *data, size_t size);
int ps4LinkWrite(int fd, const void *data, size_t size);
int ps4LinkLseek(int fd, int offset, int whence);
int ps4LinkRemove(const char *file);
int ps4LinkMkdir(const char *dirname, int mode);
int ps4LinkRmdir(const char *dirname);
int ps4LinkDopen(const char *dirname);
int ps4LinkDread(int fd, struct dirent *dir);
int ps4LinkDclose(int fd);

void ps4LinkRequestsAbort();
int ps4LinkRequestsIsConnected();
int ps4LinkGetValue(ps4LinkValue val);
int ps4LinkInit(char *serverIp, int requestPort,int debugPort, int commandPort);
void ps4LinkFinish();
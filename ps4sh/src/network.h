/*
 * ps4client host tool for PS4 providing host fileio system 
 * Copyright (C) 2003,2015 Antonio Jose Ramos Marquez (aka bigboss) @psxdev on twitter
 * Repository https://github.com/psxdev/ps4client
 * based on psp2client ps2vfs, ps2client, ps2link, ps2http tools. 
 * Credits goes for all people involved in ps2dev project https://github.com/ps2dev
 * This file is subject to the terms and conditions of the ps4client License.
 * See the file LICENSE in the main directory of this distribution for more
 * details.
 */

#ifndef __NETWORK_H__
#define __NETWORK_H__

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

///////////////////////
// NETWORK FUNCTIONS //
///////////////////////

#ifdef _WIN32
int network_startup(void);
#endif

int network_connect(char *hostname, int port, int type);

int network_listen(int port, int type);

int network_accept(int sock);

int network_send(int sock, void *buffer, int size);

int network_wait(int sock, int timeout);

int network_receive(int sock, void *buffer, int size);

int network_receive_all(int sock, void *buffer, int size);

int network_disconnect(int sock);

#endif

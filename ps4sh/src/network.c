/*
 * ps4client host tool for PS4 providing host fileio system 
 * Copyright (C) 2003,2015 Antonio Jose Ramos Marquez (aka bigboss) @psxdev on twitter
 * Repository https://github.com/psxdev/ps4client
 * based on psp2client, ps2vfs, ps2client, ps2link, ps2http tools. 
 * Credits goes for all people involved in ps2dev project https://github.com/ps2dev
 * This file is subject to the terms and conditions of the ps4client License.
 * See the file LICENSE in the main directory of this distribution for more
 * details.
 */

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#else
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#endif

#include "network.h"

///////////////////////
// NETWORK FUNCTIONS //
///////////////////////

#ifdef _WIN32
int network_startup(void) 
{
	WSADATA wsaData;

	// Start up winsock.
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) { return -1; }

	// End function.  
	return 0;

}
#endif

int network_connect(char *hostname, int port, int type) 
{ 
	int sock = -1;
	struct sockaddr_in sockaddr;

  	// Populate the sockaddr structure.
  	sockaddr.sin_family = AF_INET;
  	sockaddr.sin_port = htons(port);
  	sockaddr.sin_addr = *(struct in_addr *)gethostbyname(hostname)->h_addr;

  	// Open the socket.
  	sock = socket(AF_INET, type, 0); if (sock < 0) { return -1; }
	//int ret;
  	// Connect the socket.
	//ret=connect(sock, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr));
	//printf("connect return %d\n",ret);
	//while(ret<0)
	//{
	//	ret=connect(sock, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr));
	//	sleep(1);
	  //  printf("waiting psp2 connection...\n");
		
	//}
		if (connect(sock, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr)) < 0) { printf("error connecting\n");return -2; }

  	// Return the socket.
  	return sock;

 }

int network_listen(int port, int type) 
{ 
	int sock = -1;
  	struct sockaddr_in sockaddr;

  	// Populate the sockaddr structure.
  	sockaddr.sin_family = AF_INET;
  	sockaddr.sin_port = htons(port);
  	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  	// Create the socket.
  	sock = socket(AF_INET, type, 0); if (sock < 0) { return -1; }

  	// Bind the socket.
  	if (bind(sock, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr)) < 0) { return -1; }

  	// Return the socket.
  	return sock;

}

int network_send(int sock, void *buffer, int size) 
{ 
	int total = 0;

	// Keep sending data until it has all been sent.
	while (total < size) { total += send(sock, &((char *)buffer)[total], size - total, 0); }

  	// Return the total bytes sent.
  	return total;

}

int network_wait(int sock, int timeout) 
{
	fd_set nfds; struct timeval tv;

  	// Initialize the rdfs structure.
  	FD_ZERO(&nfds); FD_SET(sock, &nfds);

  	// Populate the tv structure.
  	tv.tv_sec = timeout; tv.tv_usec = 0;

  	// No timeout was specified, so wait forever.
  	if (timeout < 0) { return select(FD_SETSIZE, &nfds, NULL, NULL, NULL); }

  	// A timeout was specified, so wait until the time has elapsed.
  	else { return select(FD_SETSIZE, &nfds, NULL, NULL, &tv); }

  	// End function.
  	return 0;

}

int network_receive(int sock, void *buffer, int size) 
{

	// Receive the data from the socket.
	return recvfrom(sock, buffer, size, 0, NULL, NULL);

}

int network_receive_all(int sock, void *buffer, int size) 
{ 
	int total = 0;
	int ret=0;
  	// Receive the data from the socket.
  	while (total < size) 
	{ 
		ret = recvfrom(sock, &((char *)buffer)[total], size - total, 0, NULL, NULL); 
		if(ret<=0)
		{
			total=ret;
			break;
		}
		total=total+ret;
	}

  	// Return the total bytes received.
	return total;

}

int network_disconnect(int sock) 
{
  	// Close the socket.
#ifdef _WIN32
  	if (closesocket(sock) < 0) { return -1; }
#else
  	if (close(sock) < 0) { return -1; }
#endif

  	// End function.
	return 0;

}

/*
 * ps4client host tool for PS4 providing host fileio system 
 * Copyright (C) 2003,2015 Antonio Jose Ramos Marquez (aka bigboss) @psxdev on twitter
 * Repository https://github.com/psxdev/ps4client
 * based on psp2client,ps2vfs, ps2client, ps2link, ps2http tools. 
 * Credits goes for all people involved in ps2dev project https://github.com/ps2dev
 * This file is subject to the terms and conditions of the ps4client License.
 * See the file LICENSE in the main directory of this distribution for more
 * details.
 */


#ifndef __PS4LINK_H__
#define __PS4LINK_H__

///////////////////////
//  PS4LINK SOCKETS  //
///////////////////////


#define SRV_PORT    0x4711
#define CMD_PORT    0x4712
#define LOG_PORT    0x4712

///////////////////////
// PS4LINK FUNCTIONS //
///////////////////////

int ps4link_connect(char *hostname);

int ps4link_mainloop(int timeout);

int ps4link_disconnect(void);

#define PS4LINK_OPEN_CMD     0xbabe0111
#define PS4LINK_OPEN_RLY     0xbabe0112
#define PS4LINK_CLOSE_CMD    0xbabe0121
#define PS4LINK_CLOSE_RLY    0xbabe0122
#define PS4LINK_READ_CMD     0xbabe0131
#define PS4LINK_READ_RLY     0xbabe0132
#define PS4LINK_WRITE_CMD    0xbabe0141
#define PS4LINK_WRITE_RLY    0xbabe0142
#define PS4LINK_LSEEK_CMD    0xbabe0151
#define PS4LINK_LSEEK_RLY    0xbabe0152
#define PS4LINK_OPENDIR_CMD  0xbabe0161
#define PS4LINK_OPENDIR_RLY  0xbabe0162
#define PS4LINK_CLOSEDIR_CMD 0xbabe0171
#define PS4LINK_CLOSEDIR_RLY 0xbabe0172
#define PS4LINK_READDIR_CMD  0xbabe0181
#define PS4LINK_READDIR_RLY  0xbabe0182
#define PS4LINK_REMOVE_CMD   0xbabe0191
#define PS4LINK_REMOVE_RLY   0xbabe0192
#define PS4LINK_MKDIR_CMD    0xbabe01a1
#define PS4LINK_MKDIR_RLY    0xbabe01a2
#define PS4LINK_RMDIR_CMD    0xbabe01b1
#define PS4LINK_RMDIR_RLY    0xbabe01b2


int ps4link_request_open(void *packet);

int ps4link_request_close(void *packet);

int ps4link_request_read(void *packet);

int ps4link_request_write(void *packet);

int ps4link_request_lseek(void *packet);

int ps4link_request_opendir(void *packet);

int ps4link_request_closedir(void *packet);

int ps4link_request_readdir(void *packet);

int ps4link_request_remove(void *packet);

int ps4link_request_mkdir(void *packet);

int ps4link_request_rmdir(void *packet);

////////////////////////////////
// PS2LINK RESPONSE FUNCTIONS //
////////////////////////////////



int ps4link_response_open(int result);

int ps4link_response_close(int result);

int ps4link_response_read(int result, int size);

int ps4link_response_write(int result);

int ps4link_response_lseek(int result);

int ps4link_response_opendir(int result);

int ps4link_response_closedir(int result);

int ps4link_response_readdir(int result, unsigned char type, char *name);

int ps4link_response_remove(int result);

int ps4link_response_mkdir(int result);

int ps4link_response_rmdir(int result);


#define PS4LINK_EXECUSER_CMD 0xbabe0201
#define	PS4LINK_EXECKERNEL_CMD 0xbabe0202
#define	PS4LINK_EXIT_CMD 0xbabe0203
#define	PS4LINK_EXECDECRYPT_CMD 0xbabe0204
#define	PS4LINK_EXECWHOAMI_CMD 0xbabe0205
#define	PS4LINK_EXECSHOWDIR_CMD 0xbabe0206


int ps4link_command_execuser(int argc,char *argv,int argvlen);
int ps4link_command_execkernel(int argc,char *argv,int argvlen);
int ps4link_command_execdecrypt(int argc,char *argv,int argvlen);
int ps4link_command_execwhoami(int argc,char *argv,int argvlen);
int ps4link_command_execshowdir(int argc,char *argv,int argvlen);
int ps4link_command_exit(int argc,char *argv,int argvlen);

//////////////////////////////
// PS4LINK THREAD FUNCTIONS //
//////////////////////////////

void *ps4link_thread_console(void *thread_id);

void *ps4link_thread_request(void *thread_id);

////////////////////////////////
//   PS4LINK PS4SH FUNCTIONS  //
////////////////////////////////

int ps4link_fio_listener(char *dst_ip, int port, int timeout);
int ps4link_srv_setup(char *src_ip, int port);
int ps4link_log_listener(char *src_ip, int port);
void ps4link_set_debug(int level);
int ps4link_debug(void);
void ps4link_set_root(char *p);
int ps4link_set_path(char *p);

#endif


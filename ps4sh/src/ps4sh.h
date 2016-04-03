/*
 * Copyright (c) Khaled Daham
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author(s) may not be used to endorse or promote products 
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/time.h>




/* readline headers */
#include <readline/readline.h>
#include <readline/history.h>



#include "common.h"
#include "rl_common.h"
#include "debugnet.h"
#include "ps4link.h"
#include "network.h"
#include "utility.h"


#define PS4SH_VERSION "1.0"
#define MAX_SIZE        8192
#define MAX_NO_PROMPT_COUNT 20
#define MAX_SEC_BLOCK   0
#define MAX_USEC_BLOCK  100000
#define LOG_F_CREAT  (O_WRONLY | O_CREAT | O_TRUNC)
#define LOG_F_MODE  (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define USEC 1000000
#define MAX_CLIENTS 10
#define MAX_PATH    256


/* variables */
int doloop = 1;
int update_prompt = 0;
int log_f_fd = 0;
int log_to_file = 0;
int VERBOSE = 0;
int DUMPSCREEN = 0;
int state = 0;


int ps4sh_log_read(int);
int ps4sh_srv_read(int);

void read_config(void);
int initialize_readline(void);

int cli_cd(), cli_help(), cli_list(), cli_make();
int cli_pwd(), cli_quit(), cli_execee(), cli_execeiop();
int cli_reset(), cli_status(), cli_log(), cli_verbose();
int cli_setroot(), cli_debug(), cli_gsexec();
int cli_gmake(char *arg);
int cli_make(char *arg);
int cli_list(char *arg);
int cli_help(char *arg);
int cli_debug(); 
int cli_quit();
int cli_status();
int cli_execelf(char *arg);
int cli_execsprx(char *arg);
int cli_exitps4(char *arg);
int cli_execpayload(char *arg);
int cli_execwhoami(char *arg);
int cli_execshowdir(char *arg);



typedef struct {
    char *name;     /* User printable name of the function. */
   rl_icpfunc_t  *func; /* Function to call to do the job. */
    char *doc;      /* Documentation for this function.  */
} COMMAND;

COMMAND * find_command(char *);

COMMAND commands[22] = {
    { "?", cli_help, "? :: Synonym for `help'." },
    { "cd", cli_cd, "cd [dir] :: Change ps4sh directory to [dir]." },
    { "debug", cli_debug, "debug :: Show ps4sh debug messages. ( alt-d )" },
    { "exit", cli_quit, "exit :: Exits ps4sh ( alt-q )" },
    { "help", cli_help, "help :: Display this text." },
    { "list", cli_list, "list [dir] :: List files in [dir]." },
    { "log", cli_log, "log [file] :: Log messages from PS4 to [file]."},
    { "ls", cli_list, "ls [dir] :: Synonym for list" },
    { "make", cli_make, "make [argn] ... :: Execute make [argn] ..." },
    { "gmake", cli_gmake, "gmake [argn] ... :: Execute gmake [argn] ..." },	
    { "pwd", cli_pwd, "pwd :: Print the current working directory ( alt-p )" },
    { "quit", cli_quit, "quit :: Quit pksh ( alt-q )" },
    { "setroot", cli_setroot, "setroot [dir] :: Sets [dir] to be root dir." },
    { "status", cli_status, "status :: Display some ps4sh information. ( alt-s )" },
    { "execelf", cli_execelf, "execelf :: Load and exec elf. ..." },
    { "execsprx", cli_execsprx, "execsprx :: Load and exec sprx. ..." },
    { "exitps4", cli_exitps4, "exitps4 :: Finish ps4link in ps4 side. ..." },
    { "execpayload", cli_execpayload, "execpayload :: load payload in ps4 side. ..." },
    { "execwhoami", cli_execwhoami, "execwhoami :: show uid and gid in ps4 side. ..." },
    { "execshowdir", cli_execshowdir, "execshowdir :: list file from directory in ps4 side. ..." },
    { "verbose", cli_verbose, "verbose :: Show verbose pksh messages. ( alt-v )" },
	{ (char *)NULL, (rl_icpfunc_t *)NULL, (char *)NULL }
};



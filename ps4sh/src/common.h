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

#ifndef _COMMON
#define _COMMON
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef __WIN32__
#include <sys/param.h>
#endif


/* readline headers */
#ifndef __WIN32__
#include <readline/readline.h>
#include <readline/history.h>
#endif

#ifdef __WIN32__
#define MAXPATHLEN 1024
#endif
#define NEWLINE '\n'

#define MAX_ARGV 256

extern char **environ;

static const int DUMP_REG_MAX = 12;
static const char DUMP_REG_SYM[13][6] = {
	"dma", "intc", "timer", "gs", "sif", "fifo",
	"gif", "vif0", "vif1", "ipu", "all", "vu0", "vu1"
};

char *trim(char *);
char *stripwhite();
char * dupstr(char *s);
int fix_cmd_arg(char *argv, const char *cmd, int *argvlen);
void read_pair(char *, char *, char *);
int read_line(FILE *, char *);
int get_home(char *);
int arg_device_check(char *);
int argv_split(char *, const char *);
int size_file(char *);
int read_file(char *, unsigned char *, unsigned int);
void arg_prepend_host(char *new, char *old);
int get_register_index(char *str, int size);
int build_argv(char *argv[], char *arg);
void free_argv(char *argv[], int argc);
void split_filename(char *device, char *dir, char *filename, const char *arg);
#endif

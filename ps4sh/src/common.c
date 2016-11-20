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
#include "common.h"

static int is_file(char *file);

void read_pair(char *line, char *k, char *v) 
{
	char *s = "=";
	char *whole;
	whole = strsep(&line, s);
	strcpy(k, whole);
	if (line != NULL) 
	{
		strcpy(v, line);
	} 
	else 
	{
		strcpy(v, "");
	}
	return;
}

int read_line(FILE *fd, char *line) 
{
	int i = 0;
	char c = 0;
	while((c != NEWLINE) && (i < MAXPATHLEN)) 
	{
		c = getc(fd);
		if (feof(fd)) 
		{
			return(-1);
		}
		*line++ = c;
		++i;
	}
	if (*line == NEWLINE) 
	{
	}
	*--line = (char)NULL;
	return(0);
}

int get_home(char *line) 
{
	int i;
	for(i = 0; environ[i] != NULL; i++) 
	{
		if(!strncmp(environ[i], "HOME=", 5)) 
		{
			strcpy(line, environ[i]);
			return 1;
		}
	}
	return 0;
}

char * trim(char *s) 
{
	int len = strlen(s);
	int i = len;
	while (i > 0) 
	{
		if (s[i-1] != ' ') 
		{
			break;
		}
		i--;
	}
	while(len > 0) 
	{
		if (*s != ' ') 
		{
			break;
		}
		s++;
		len--;
	}
	s[i] = '\0';
	return s;
}

char * dupstr(char *s)
{
	char *r;
	r = malloc(strlen(s) + 1);
	strcpy(r, s);
	return(r);
}

char * stripwhite(char *string)
{
	register char *s, *t;
	for (s = string; whitespace(*s); s++)
		;

 	if (*s == 0) 
	{
		return(s);
	}
	t = s + strlen(s) - 1;
	while(t > s && whitespace(*t)) 
	{
		t--;
	}
	*++t = '\0';
	return s;
}

int arg_device_check(char *arg) 
{
	if ((strstr(arg, ":/")) != NULL ) 
	{
		return 1;
	} 
	else if ((strstr(arg, "host0:")) != NULL ) 
	{
		return 1;
	} 
	else if ((strstr(arg, ":")) != NULL ) 
	{
		return 1;
	}
	return 0;
}

void arg_prepend_host(char *new, char *old) 
{
	char *pstr_ptr;
	if ((pstr_ptr = strstr(old, ":/")) != NULL ) 
	{
		(void)strcpy(new, old);
	} 
	else if ((pstr_ptr = strstr(old, "host0:")) != NULL ) 
	{
		pstr_ptr = strstr(pstr_ptr, ":");
		++pstr_ptr;
 	   (void)strcpy(new, "host0:");
	   (void)strcat(new, pstr_ptr);
	} 
	else if ((pstr_ptr = strstr(old, ":")) != NULL ) 
	{
		(void)strcpy(new, old);
	} 
	else 
	{
		(void)strcpy(new, "host0:");
		(void)strcat(new, old);
	}
}

int fix_cmd_arg(char *argv, const char *cmd, int *argvlen) 
{
	char arg[MAXPATHLEN];
	int argc;
	int ai, pi, ac;
	ai = 0;
	pi = 0;
	ac = 0;
	argc = 0;
	while (cmd[pi]) 
	{
		while ((cmd[pi] != '\0') && (cmd[pi] != ' ')) 
		{
			pi++;
		}
		memcpy(arg, &cmd[ai], pi-ai);
		arg[pi-ai] = '\0';
		if ( is_file(arg) != 0 ) 
		{
			if ( !arg_device_check(arg) ) 
			{
				
				memcpy(&argv[ac], "host0:", 6);
				memcpy(&argv[ac+6], arg, strlen(arg));
				ac += strlen(arg)+6;
			} 
			else 
			{				
				memcpy(&argv[ac], arg, strlen(arg));
				ac += strlen(arg);
			}
		} 
		else 
		{				
			memcpy(&argv[ac], arg, strlen(arg));
			ac += strlen(arg);
		}
		argv[ac] = '\0';
		ac++;
		ai = pi;
		ai++;
    
		argc++;
		while ((cmd[pi]) && (cmd[pi] == ' ')) 
		{
			pi++;
		}
	}
	*argvlen = ac;
	return argc;
}
int fix_cmd_arg_non_host(char *argv, const char *cmd, int *argvlen) 
{
	char arg[MAXPATHLEN];
	int argc;
	int ai, pi, ac;
	ai = 0;
	pi = 0;
	ac = 0;
	argc = 0;
	while (cmd[pi]) 
	{
		while ((cmd[pi] != '\0') && (cmd[pi] != ' ')) 
		{
			pi++;
		}
		memcpy(arg, &cmd[ai], pi-ai);
		arg[pi-ai] = '\0';
			
			memcpy(&argv[ac], arg, strlen(arg));
			ac += strlen(arg);
		
		argv[ac] = '\0';
		ac++;
		ai = pi;
		ai++;
    
		argc++;
		while ((cmd[pi]) && (cmd[pi] == ' ')) 
		{
			pi++;
		}
	}
	*argvlen = ac;
	return argc;
}

int read_file(char *file, unsigned char *data, unsigned int size) 
{
	FILE *fd = fopen(file, "rb");
	if (fread(data, size, 1, fd) != size) 
	{
		return -1;
	}
	return size;
}

int size_file(char *file) 
{
	struct stat finfo;
	if (stat(file, &finfo) != 0) 
	{
		return 0;
	}
	return (finfo.st_size);
}

static int is_file(char *file) 
{
	struct stat finfo;
	if (stat(file, &finfo) != 0) 
	{
		return 0;
	}
	return (S_ISREG(finfo.st_mode));
}

void split_filename(char *device, char *dir, char *filename, const char *arg) 
{
	const char *ptr;
	memset(device, 0, MAXPATHLEN);
	memset(dir, 0, MAXPATHLEN);
	memset(filename, 0, MAXPATHLEN);
	// check for device name
	if ((ptr = strstr(arg, ":")) != NULL ) 
	{
		if ( (ptr - arg) > MAXPATHLEN ) 
		{
			device[0] = '\0';
		} 
		else 
		{
			strncpy(device, arg, ptr-arg+1);
			device[ptr-arg+1] = '\0';
		}
		ptr++;
	} 
	else 
	{
		device[0] = '\0';
		ptr = arg;
	}
	// check for dir part

	// check for filename part
	strncpy(filename, ptr, MAXPATHLEN);
	return;
}

int get_register_index(char *str, int size) 
{
	int i;
	for(i = 0; i < DUMP_REG_MAX; i++) 
	{
		if(!strncmp(str, DUMP_REG_SYM[i], size) ) 
		{
			break;
		}
	}
	return i;
}

int build_argv(char *argv[], char *arg) 
{
	int i, ai;
	char *aptr = arg;

	if ((arg == NULL) || (strlen(arg) == 0)) 
	{
		argv[0] = NULL;
		return 0;
	}

	for(i = 0; i < MAX_ARGV; i++) 
	{
		ai = 0;
		while ((aptr[ai] != '\0') && (aptr[ai] != ' ')) 
		{
			ai++;
		}

		argv[i] = malloc(ai);
		strncpy(argv[i], aptr, ai);
		argv[i][ai] = 0;
		aptr = aptr + ai;

		while (aptr[0] == ' ') 
		{
			aptr++;
		}
		if(aptr[0] == '\0') 
		{
			break;
		}
	}
	return i+1;
}

void free_argv(char *argv[], int argc) 
{
	int i;
	for(i = 0; i < argc; i++) {
		free(argv[i]);
	}
	return;
}

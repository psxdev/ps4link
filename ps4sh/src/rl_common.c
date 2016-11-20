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
#include "rl_common.h"
#include "list.h"

static int cprompt = 1;
static int lprompt = 1;
static char prompt_buf[40];
static char line_buf[256];
static llist suffix_list;
static int accept_suffix(const char *);
static int is_dir(const char *);

char ** command_completion(const char *text,int start,int end)
{
	char **matches;
	matches = (char **)NULL;
	if (start == 0) 
	{
		matches = rl_completion_matches(text, &command_generator);
	}
	else
	{
		rl_bind_key('\t',rl_abort);
	}
	return(matches);
}

int filename_completion_ignore(char **names)
{
	char **newnames;
	int idx, nidx;

	if (names[1] == (char *)0) 
	{
		return -1;
	}

	/* Allocate space for array to hold list of pointers to matching
	   filenames.  The pointers are copied back to NAMES when done. */
	for (nidx = 1; names[nidx]; nidx++)
		;
	newnames = (char **)malloc((nidx+1)*sizeof(char *));

	newnames[0] = names[0];
	for (idx = nidx = 1; names[idx]; idx++) 
	{
		if (accept_suffix(names[idx]) == 0 ) 
		{
			newnames[nidx++] = names[idx];
		} 
		else if (is_dir(names[idx]) == 1)  
		{
			newnames[nidx++] = names[idx];
		} 
		else 
		{
			free(names[idx]);
		}
	}

	newnames[nidx] = (char *)NULL;

	if (nidx == 1) 
	{
		free(names[0]);
		names[0] = (char *)NULL;
		free(newnames);
		return -1;
	}

	if (nidx == 2) 
	{
		free (names[0]);
		names[0] = newnames[1];
		names[1] = (char *)NULL;
		free(newnames);
		return -1;
	}

	/* Copy the acceptable names back to NAMES, set the new array end,
	   and return. */
	for (nidx = 1; newnames[nidx]; nidx++)
		names[nidx] = newnames[nidx];
	names[nidx] = (char *)NULL;
	free (newnames);
	return 0;
}

void common_set_suffix(llist ptr) 
{
	suffix_list = ptr;
}

static int accept_suffix(const char *name)
{
	llist sfl = suffix_list;

	if (!sfl) 
	{
		return 0;
	}

	while (sfl) 
	{
		if ( strstr(name, sfl->dir) ) 
		{
			return 0;
		}
		sfl = sfl->next;
	}
	return 1;
}

llist path_split(char *line) 
{
	char *sep = ":";	
	char *dir, *brkt;
	llist list, tail = NULL;
	for(dir = strtok_r(line, sep, &brkt);
	dir;
	dir = strtok_r(NULL, sep, &brkt))
	{
		list = (llist)malloc(sizeof(llist));
		list->dir = (char *)malloc(strlen(dir));
		strcpy(list->dir, dir);
		list->next = tail;
		tail = list;
	}
	list = tail;
	return list;
}

int change_prompt(void) 
{
	cprompt = !cprompt;
	strcpy(line_buf, rl_line_buffer);
	rl_callback_handler_install(get_prompt(), cli_handler);
	rl_insert_text(line_buf);
	rl_refresh_line(0, 0);
	return(0);
}

int write_log_line(char *buf) 
{
	lprompt = !lprompt;
	strcpy(line_buf, rl_line_buffer);
	rl_callback_handler_install(log_prompt(), cli_handler);
	rl_refresh_line(0, 0);
	printf("%s",buf);
	lprompt = !lprompt;
	rl_on_new_line();
	rl_callback_handler_install(log_prompt(), cli_handler);
	rl_refresh_line(0, 0);
	return(0);
}

char * log_prompt(void) 
{
	sprintf(prompt_buf, "%s", lprompt ? "ps4sh> ": "log: ");
	return prompt_buf;
}

char * get_prompt(void) 
{
	sprintf(prompt_buf, "%s", cprompt ? "ps4sh> ": "peer away> ");
	return prompt_buf;
}


void cli_handler(char *name) 
{
	if (*rl_line_buffer && rl_line_buffer != NULL)
	{
		add_history(rl_line_buffer);
		execute_line(rl_line_buffer);
	}
}

static int is_dir(const char *name)
{
	struct stat finfo;
	if (stat(name, &finfo) != 0) 
	{
		return 0;
	}
	return (S_ISDIR(finfo.st_mode));
}

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

#ifndef __UTILITY_H__
#define __UTILITY_H__

#define PACKED __attribute__((packed))

///////////////////////
// UTILITY FUNCTIONS //
///////////////////////

int fix_flags(int flags);

int fix_pathname(char *pathname);

int fix_argv(char *destination, char **argv);

int print_usage(void);

#endif

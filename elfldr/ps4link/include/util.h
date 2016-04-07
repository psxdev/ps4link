#ifndef Util_H
#define Util_H

#include <stdlib.h>
#include <stdio.h>

void utilStandardIORedirect(int to, int stdfd[3], fpos_t stdpos[3]);
void utilStandardIOReset(int stdfd[3], fpos_t stdpos[3]);

int utilServerCreate(int port, int backlog, int try, unsigned int sec);
int utilSingleAcceptServer(int port);

void *utilAllocUnsizeableFileFromDescriptor(int fd, size_t *size);
void *utilAllocFileAligned(char *file, size_t *size, size_t alignment);
#define utilAllocFile(file, size) utilAllocFileAligned(file, size, 1)

#endif

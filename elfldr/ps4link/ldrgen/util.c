#define _XOPEN_SOURCE 700
#define __BSD_VISIBLE 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <netinet/in.h>
#include "util.h"


FILE *fddupopen(int fd, const char *mode)
{
	int t;
	FILE *r = NULL;

	if(mode == NULL)
		return NULL;

	if((t = dup(fd)) < 0)
		return NULL;

	if((r = fdopen(t, mode)) == NULL)
		close(t);

	return r;
}


void *utilAllocUnsizeableFileFromDescriptor(int fd, size_t *size)
{
	int length = 0;
	int full = 4096;
	uint8_t *data = (void *)malloc(full);
	size_t s = 0;

	if(size != NULL)
		*size = 0;

	while((length = read(fd, data + s, full - s)) > 0)
	{
		s += length;
		if(s == full)
		{
			void *t;
			full *= 2;
			t = malloc(full);
			memcpy(t, data, s);
			free(data);
			data = t;
		}
	}

	if(size != NULL)
		*size = s;

	return data;
}

void *utilAllocFileAligned(char *file, size_t *size, size_t alignment)
{
	struct stat s;
	FILE *f;
	uint32_t *b;
	size_t sz;
	size_t i;

	if(size != NULL)
		*size = 0;

	if(stat(file, &s) < 0)
		return NULL;

	if(alignment == 0)
		alignment = 1;

 	sz = ((size_t)s.st_size * alignment) / alignment;
	b = (uint32_t *)malloc(sz * sizeof(uint8_t));

	if(b == NULL)
		return NULL;

	f = fopen(file, "rb");
	if(f == NULL)
	{
		free(b);
		return NULL;
	}
	fread(b, s.st_size, 1, f);
	fclose(f);

	if(size != NULL)
		*size = sz;

	for(i = s.st_size; i < sz; ++i)
		((uint32_t *)b)[i] = 0;

	return b;
}

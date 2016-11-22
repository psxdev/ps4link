#define _XOPEN_SOURCE 700
#define __BSD_VISIBLE 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

enum{ ELF_LOADER_BINARY_SEGMENT_SIZE = 0x100000 };

void *elfLoaderMemoryAllocateFileFromPathAligned(char *file, size_t *size, size_t alignment)
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

int main(int argc, char **argv)
{
	uint32_t *bin, *text, *data;
	off_t binSize, textSize, dataSize;
	size_t s;
	FILE *f;
	int i;

	if(argc < 3)
	{
		fprintf(stderr, "Wrong number or arguments (%i):\t %s <name> <binary in> <js out>\n", argc, argv[0]);
		return EXIT_FAILURE;
	}

	bin = elfLoaderMemoryAllocateFileFromPathAligned(argv[2], &s, 4);
	if(bin == NULL)
	{
		fprintf(stderr, "Bin %s could not be loaded\n", argv[2]);
		return EXIT_FAILURE;
	}
	binSize = (off_t)s;
	text = bin;
	textSize = binSize;
	data = NULL;
	dataSize = 0;

	if(binSize > ELF_LOADER_BINARY_SEGMENT_SIZE)
	{
		for(i = (ELF_LOADER_BINARY_SEGMENT_SIZE - 1) / 4; i > 0 && bin[i] == 0; --i);
		textSize = (i + 1) * 4;
		data = (uint32_t *)((uint8_t *)bin + ELF_LOADER_BINARY_SEGMENT_SIZE);
		dataSize = binSize - ELF_LOADER_BINARY_SEGMENT_SIZE;
	}

	f = fopen(argv[3], "wb");
	//fprintf(f, "function %s()\n{\n", argv[1]);
	fprintf(f, "function Ldr()\n{\n");
	if(strncmp(argv[1], "Elf", 3) == 0)
		fprintf(f, "\tthis.isElfLoader = function()\n\t{\n\t\treturn true;\n\t}\n");
	else
		fprintf(f, "\tthis.isElfLoader = function()\n\t{\n\t\treturn false;\n\t}\n");
	fprintf(f, "\tthis.writeData = function(address)\n\t{\n\t\tsetBase(address);\n");
	for(i = 0; i < dataSize / 4; i++)
		fprintf(f, "\t\tu32[%i] = 0x%08x;\n", i, data[i]);
	fprintf(f, "\t}\n\n");
	fprintf(f, "\tthis.writeText = function(address)\n\t{\n\t\tsetBase(address);\n");
	for(i = 0; i < textSize / 4; i++)
		fprintf(f, "\t\tu32[%i] = 0x%08x;\n", i, text[i]);
	fprintf(f, "\t}\n");
	fprintf(f, "}\n");
	fclose(f);

	free(bin);

	printf("Generated %s in %s from bin %s\n", argv[1], argv[3], argv[2]);

	return EXIT_SUCCESS;
}

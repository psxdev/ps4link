#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#include "util.h"

enum{ BinarySegmentSize = 0x100000 };

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

	bin = utilAllocFileAligned(argv[2], &s, 4);
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

	if(binSize > BinarySegmentSize)
	{
		for(i = (BinarySegmentSize - 1) / 4; i > 0 && bin[i] == 0; --i);
		textSize = (i + 1) * 4;
		data = (uint32_t *)((uint8_t *)bin + BinarySegmentSize);
		dataSize = binSize - BinarySegmentSize;
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

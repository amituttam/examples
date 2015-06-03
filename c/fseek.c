#include <stdlib.h>
#include <stdio.h>

#define BIOS_VERSION_OFFSET 892460

int main( int argc, char **argv )
{
	FILE *bios;
	char bios_file_name[16];

	bios = fopen(argv[1], "rb");
	fseek(bios, BIOS_VERSION_OFFSET, SEEK_SET);
	fread(bios_file_name, sizeof(bios_file_name), 1, bios);
	bios_file_name[8] = '\0';
	printf("BIOS File Name: %s\n", bios_file_name);
	fclose(bios);
	return 0;
}

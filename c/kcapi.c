
#include <stdio.h>
#include <unistd.h>
#include <kcapi.h>

void pt(unsigned char *md)
{
	int i;

	for (i=0; i<20; i++)
		printf("%02x",md[i]);
	printf("\n");
}

int main( int argc, char **argv )
{
	int i;
	FILE *IN;
	int ret = 0;
	struct kcapi_handle handle;
	unsigned char md[20];
	unsigned char buf[4096] __attribute__((__aligned__(4096)));

	ret = kcapi_md_init(&handle, "sha1");
	if (ret) {
		printf("Allocation of sha1 cipher failed (ret=%d)\n", ret);
		return 1;
    }
	else {
		int ds = kcapi_md_digestsize(&handle);
		printf("Digest Size=%d\n", ds);
	}

	/* Read the file specified by user */
	IN = fopen(argv[1],"r");
	if (IN == NULL)
	{
		perror(argv[1]);
		return 1;
	}

	/* Read to buffer */
	int fd;
	fd = fileno(IN);

	for (;;)
	{
		i = read(fd, buf, 4096);
		if (i <= 0) break;
		kcapi_md_update(&handle,buf,(unsigned long)i);
	}
	kcapi_md_final(&handle, &(md[0]), 20);
	pt(md);

	/* Clean up */
	fclose(IN);
	kcapi_md_destroy(&handle);

	return ret;
}

/* colorgcc -Wall -lkcapi -o kcapitest kcapi.c */

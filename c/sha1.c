#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <openssl/sha.h>

#define BUFSIZE	1024*16

int read(int, void *, unsigned int);
int main(int argc, char *argv[])
{
	int i;
	int rc;
	SHA_CTX c;
	FILE *in;
	unsigned long size = 0;
	unsigned char buf[BUFSIZE];
	unsigned char md[SHA_DIGEST_LENGTH];

	/* Allocate memory */
	size = 100000000;
	char *mem = malloc(size);
	memset(mem, 16, 100);

	/* Open the file for reading */
#if 0
	printf("Computing SHA1SUM on %s\n", argv[1]);
	in = fopen(argv[1], "r");
	if (in == NULL)
	{
		printf("Failed to open %s for reading\n", argv[1]);
		exit(1);
	}
#endif

	/* Initialize SHA context */
	rc = SHA1_Init(&c);
	if (!rc)
	{
		printf("SHA1_Init failed\n");
		exit(1);
	}

	SHA1_Update(&c, mem, size);
#if 0
	/* Update the context with all the data */
	for (;;)
	{
		/* Read BUFSIZE chunk */
		i = read(fileno(in), buf, BUFSIZE);

		/* If no more bytes read then quit loop */
		if (i <= 0)
			break;

		/* Update context */
		rc = SHA1_Update(&c, buf, (unsigned long) i);
		if (!rc)
		{
			printf("SHA1_Update failed\n");
			exit(1);
		}
		size = size + BUFSIZE;
	}
#endif
	printf("SHA1_Update finished\n");
	//printf("Bytes read: %lu\n", size);

	/* Compute the SHA1 and save the message digest */
	rc = SHA1_Final(&(md[0]), &c);
	if (!rc)
	{
		printf("SHA1_Final failed\n");
		exit(1);
	}
	printf("SHA1_Final finished\n");

	/* Print out the digest */
	for (i = 0; i < SHA_DIGEST_LENGTH; i++)
		printf("%02x", md[i]);
	printf("\n");

	/* Done */
	free(mem);
	//fclose(in);
	return 0;
}

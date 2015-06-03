#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>
#include <openssl/sha.h>

#define BUFSIZE	1024*16
#define CPU 8
#define SIZE 100000000

int read(int, void *, unsigned int);

struct thread_args
{
	char *data;
	int cpu;
};

static void *compute_sha1 (void *arg)
{
	int i;
	int rc;
	SHA_CTX c;
	unsigned char md[SHA_DIGEST_LENGTH];
	char *mem = (char *) arg;

	/* Initialize SHA context */
	rc = SHA1_Init(&c);
	if (!rc)
	{
		printf("SHA1_Init failed\n");
		exit(1);
	}

	SHA1_Update(&c, mem, SIZE);

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

	return NULL;
}

int main(int argc, char *argv[])
{
	int i;
	int rc;
	unsigned long size = 0;
	pthread_t sha1_thread[CPU];
	pthread_attr_t attr;
	cpu_set_t cpu_set;

	/* Allocate memory */
	size = SIZE;
	char *mem = malloc(size);
	memset(mem, 16, 100);

	/* Initialize thread attribute object */
	pthread_attr_init(&attr);

	for (i = 0; i < CPU; i++)
	{
		/* Set up thread to execute on CPU */
		CPU_ZERO(&cpu_set);
		CPU_SET(i, &cpu_set);
		pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpu_set);

		if (pthread_create(&sha1_thread[i], &attr, &compute_sha1, mem))
			printf("Creating thread %d failed", i);
	}

	/* Wait for all threads to finish */
	for (i = 0; i < CPU; i++)
		pthread_join(sha1_thread[i], NULL);

	/* Clean up */
	pthread_attr_destroy(&attr);

	/* Done */
	free(mem);
	return 0;
}

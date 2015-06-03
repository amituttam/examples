#include <stdlib.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
   int j, r, nloops;
   unsigned int seed;
   FILE *fp;

   if (argc != 2) {
	   fprintf(stderr, "Usage: %s <nloops>\n", argv[0]);
	   exit(EXIT_FAILURE);
   }

   fp = fopen("/dev/urandom", "r");
   if (!fp)
   {
	   perror("urandom");
	   exit(EXIT_FAILURE);
   }

   fread(&seed, sizeof(int), 1, fp);

   nloops = atoi(argv[1]);

   srand(seed);
   for (j = 0; j < nloops; j++) {
	   r =  rand();
	   printf("%d\n", r);
   }

   fclose(fp);

   exit(EXIT_SUCCESS);
}

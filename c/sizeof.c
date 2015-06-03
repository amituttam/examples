/****************************************************************************
 *
 * Purpose: Find out the size of the different data types
 * Author:  M J Leslie
 * Date:    27-feb-94
 *
 ****************************************************************************/

#include <stdio.h>
#include <unistd.h>

typedef enum {
	AIO_SUCCESS,
	AIO_CFG_ERROR,
	AIO_SRC_IO_ERROR,
	AIO_DST_IO_ERROR,
	AIO_IOCB_ERROR,
	AIO_READ_ERROR,
	AIO_WRITE_ERROR,
	AIO_IO_SUBMIT_ERROR,
	AIO_MEM_ERROR,
	AIO_HASH_ERROR,
	AIO_INDIVIDUAL_WRITE_ERROR,
	AIO_IMAGE_CORRUPT_ERROR,
	AIO_FS_INVALID_ERROR,
	AIO_FS_BOOTSECTOR_READ_ERROR,
	AIO_FS_ERROR,
	AIO_ABORTED,
	AIO_IMAGE_INIT_ERROR,
} core_ret_E;

main()
{
	int j = 10;
	int *pj = &j;

	(*pj)--;
	printf("pj = %d\n", *pj);

	char test[24];
	snprintf(test, sizeof(test), "TEST");
	char *testp = test;
	printf("SIZE OF test: %d\n", sizeof((*testp)));
	snprintf(testp, sizeof(test), "TEST2");
	printf("testp: %s\n", testp);


					/*
					 * int/char   are 'type specifiers'
					 * short/long are 'type qualifiers'
					 */
  int i;
  pid_t pid;

  printf("    short int   is %2d bytes \n", sizeof(short int));
  printf("          int   is %2d bytes \n", sizeof(int));
  printf("        int *   is %2d bytes \n", sizeof(int *));
  printf("     long int   is %2d bytes \n", sizeof(long int));
  printf("   long int *   is %2d bytes \n", sizeof(long int *));
  printf("   signed int   is %2d bytes \n", sizeof(signed int));
  printf(" unsigned int   is %2d bytes \n", sizeof(unsigned int));
  printf(" unsigned int * is %2d bytes \n", sizeof(unsigned int *));
  printf("\n");
  printf("        float   is %2d bytes \n", sizeof(float));
  printf("      float *   is %2d bytes \n", sizeof(float *));
  printf("       double   is %2d bytes \n", sizeof(double));
  printf("     double *   is %2d bytes \n", sizeof(double *));
  printf("  long double   is %2d bytes \n", sizeof(long double));
  printf("\n");
  printf("  signed char   is %2d bytes \n", sizeof(signed char));
  printf("         char   is %2d bytes \n", sizeof(char));
  printf("       char *   is %2d bytes \n", sizeof(char *));
  printf("unsigned char   is %2d bytes \n", sizeof(unsigned char));
  printf("core_ret_E      is %2d bytes \n", sizeof(core_ret_E));
  printf("pid_t           is %2d bytes \n", sizeof(pid_t));
}


/************************************************************* 
 *
 *	The O/P from this program is 
 *
 *     short int is  2 bytes 
 *           int is  4 bytes 
 *         int * is  4 bytes 
 *      long int is  4 bytes 
 *    long int * is  4 bytes 
 *    signed int is  4 bytes 
 *  unsigned int is  4 bytes 
 * 
 *         float is  4 bytes 
 *       float * is  4 bytes 
 *        double is  8 bytes 
 *      double * is  4 bytes 
 *   long double is 12 bytes 
 * 
 *   signed char is  1 bytes 
 *          char is  1 bytes 
 *        char * is  4 bytes 
 * unsigned char is  1 bytes 
 * 
 *
 **************************************************************/

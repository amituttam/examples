#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>

typedef enum {
	DTYPE_MAGIC = 0,
	DTYPE_CRYPT_FLAG,
	DTYPE_CIPHER,
    DTYPE_CIPHERMODE,
    DTYPE_IVGEN,
    DTYPE_PASSWD,
	DTYPE_PW_DIGEST,
	DTYPE_END_RESERVED
} data_type;

typedef struct tlv_msgs
{
	data_type datatype;     // datatypes - 6 types for now
	u_int32_t bytelen;      // no. bytes in data part
	u_int8_t  *data;
} tlv_msg;

int main( int argc, char **argv )
{
#if 0
  char* arr  = malloc(10);
  int*  arr2 = malloc(sizeof(int));

  memset(arr, 0, 10);
  write( 1 /* stdout */, arr, 10 );
  free(arr);
  exit(arr2[0]);
#endif

  	int sz = atoi(argv[1]);

	tlv_msg *decode_msg = NULL;
	decode_msg = (tlv_msg*) malloc(sizeof(tlv_msg));

	printf("Size of data_type=%d\n", (int) sizeof(decode_msg->datatype));
	printf("Size of u_int32_t=%d\n", (int) sizeof(decode_msg->bytelen));
	printf("Size of u_int8_t=%d\n", (int) sizeof(decode_msg->data));

	decode_msg->data = (unsigned char*) calloc(1, sz);

	free(decode_msg->data);
	free(decode_msg);

	return 0;
}

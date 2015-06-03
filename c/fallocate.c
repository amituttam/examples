#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main( int argc, char **argv )
{
	int dstfd;
	int rc = -1;
	char *dstname = argv[1];
	//int dst_oflags = O_CREAT | O_WRONLY | O_LARGEFILE | O_DIRECT;
	int dst_oflags = O_CREAT | O_WRONLY | O_LARGEFILE;

	printf("dstname: %s\n", dstname);

	if ( ( dstfd = open( dstname, dst_oflags, 0666 ) ) < 0 ) {
			perror( dstname );
			goto out;
	}

	if ( dst_oflags & O_CREAT ) {
			rc = fallocate( dstfd, 0, 0, 4294967296ULL );
			if ( rc )
					printf( "fallocate failed: %s\n", strerror( errno ) );
			goto out;
	}

	rc = 0;

out:
	return rc;
}

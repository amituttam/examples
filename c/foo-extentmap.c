#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fs.h>
#include <linux/fiemap.h>
#include <unistd.h>

#define MAX_EXTENTS	10

typedef struct _file_ext {
	__u64 sector;	/* physical offset in bytes for the start of the extent from the beginning of the disk */
	__u64 length;	/* length in bytes for this extent */
	__u64 free;	/* length in remaining in this extent */
} file_ext_S;

typedef struct _filemap {
	file_ext_S	**blk;
	int		nblks;
} filemap_S;

static int
my_fallocate( const char *filename, off_t length )
{
	int rc = -1;
	int fd = -1;
	
	fd = open( filename, O_RDWR | O_CREAT );
	if ( 0 >= fd ) {
		perror( "open" );
		goto out;
	}
#if 0
        rc = posix_fallocate( fd, 0, length );
        if ( rc ) {
                printf( "posix_fallocate failed: %s\n", strerror( rc ) );
        }
	goto out;
#endif

	if ( -1 == lseek( fd, length, SEEK_SET ) ) {
		perror( "lseek" );
		goto out;
	}
	else {
		write( fd, "Hello World", 11 );
		if ( -1 == ftruncate( fd, length ) ) {
			perror( "ftruncate" );
			goto out;
		}
	}
	rc = 0;
out:
	if ( fd ) close( fd );

	return rc;
}

static int
create_filemap( filemap_S *fmap, int nblks )
{
	int ii;
	int err;

	fmap->blk = calloc( nblks, sizeof( filemap_S * ) );
	if ( !fmap->blk ) {
		err = errno;
		printf( "Failed to allocate memory: '%s'", strerror( err ) );
		return -1;
	}

	fmap->nblks = nblks;

	for ( ii = 0; ii < nblks; ii++ ) {
		fmap->blk[ ii ] = calloc( 1, sizeof( filemap_S ) );
		if ( !fmap->blk[ ii ] ) {
			err = errno;
			printf( "Failed to allocate memory: '%s'", strerror( err ) );
			return -1;
		}
	}

	return 0;
}

static void
destroy_filemap( filemap_S *fmap )
{
	int ii;

	for ( ii = 0; ii < fmap->nblks; ii++ ) {
		if ( fmap->blk[ ii ] )
			free( fmap->blk[ ii ] );
	}

	if ( fmap->blk ) free( fmap->blk );
}


static void
print_filemap( filemap_S *fmap )
{
	int ii;

	for ( ii = 0; ii < fmap->nblks; ii++ ) {
		printf( "%d: sector=%llu length=%llu, diff=%llu\n", ii, fmap->blk[ ii ]->sector,
									fmap->blk[ ii ]->length,
									(ii > 0)?fmap->blk[ ii ]->sector - fmap->blk[ ii-1 ]->sector:0 );
	}
}

int
get_filemap( filemap_S *map, int fd, uint64_t length )
{
	struct fiemap *fmap = NULL;
	int err;
	int nexts;
	int ii;
	int idx = 0;
	int rc;

	fmap = calloc( 1, sizeof( struct fiemap ) );
	if ( !map ) {
		err = errno;
		printf( "Failed to allocate memory: '%s'", strerror( err ) );
		return -1;
	}

	fmap->fm_start = 0;
	fmap->fm_length = length;
	fmap->fm_extent_count = 0;

	rc = ioctl( fd, FS_IOC_FIEMAP, fmap );
	if ( -1 == rc ) {
		perror( "get_filemap: ioctl" );
		goto out;
	}
	printf( "rc=%d, nexts=%d\n", rc, fmap->fm_mapped_extents );
	nexts = fmap->fm_mapped_extents;

	rc = create_filemap( map, nexts );
	if ( -1 == rc ) {
		printf( "failed to create map\n" );
		goto out;
	}

	fmap = realloc( fmap, sizeof( struct fiemap ) + ( MAX_EXTENTS * sizeof( struct fiemap_extent ) ) );
	fmap->fm_start = 0;
	fmap->fm_length = length;

	do {
		fmap->fm_extent_count = MAX_EXTENTS;
		rc = ioctl( fd, FS_IOC_FIEMAP, fmap );
		if ( -1 == rc ) {
			perror( "ioctl" );
			goto out;
		}
		printf( "rc=%d, nexts=%d, %llu - %llu\n", rc, fmap->fm_mapped_extents,
								fmap->fm_start,
								fmap->fm_length );

		for ( ii = 0; ii < fmap->fm_mapped_extents; ii++, idx++ ) {
			map->blk[ idx ]->sector = fmap->fm_extents[ ii ].fe_physical;
			map->blk[ idx ]->length = fmap->fm_extents[ ii ].fe_length;
			map->blk[ idx ]->free = fmap->fm_extents[ ii ].fe_length;

		}

		rc = fmap->fm_mapped_extents - 1;
		fmap->fm_start = fmap->fm_extents[ rc ].fe_logical + fmap->fm_extents[ rc ].fe_length;
		fmap->fm_length = length - fmap->fm_start;

		nexts -= fmap->fm_mapped_extents;
	} while( 0 < nexts );
	rc = 0;

out:
	if ( fmap ) free( fmap );

	return rc;
}

int main( int argc, char **argv )
{
	int fd;
	uint64_t length = 4294967296ULL;
	filemap_S fmap;
	int rc;

	if ( argc != 2 ) {
		printf( "Usage: %s <filename>\n", argv[ 0 ] );
		return 1;
	}

	/*
	if ( 0 != my_fallocate( argv[1], length ) ) 
		printf( "my_fallocate failed\n" );
	else
		printf( "my_fallocate success\n" );
	*/

	fd = open( argv[ 1 ], O_RDWR | O_CREAT );
	if ( 0 >= fd ) {
		perror( "open" );
		return 1;
	}
	
	/*
	rc = posix_fallocate( fd, 0, length );
	if ( rc ) {
		printf( "posix_fallocate failed: %s\n", strerror( rc ) );
		goto out;
	}
	*/

	rc = get_filemap( &fmap, fd, length );
	if ( 0 == rc )
		print_filemap( &fmap );

	destroy_filemap( &fmap );

out:
	close( fd );

	return 0;
}

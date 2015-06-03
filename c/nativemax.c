
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>

/*
 * This IOCTL was introduced to use the libata
 * facilities in the kernel to unlock HPA
 */
#define HDIO_REFRESH_DISK_CAPACITY  0x0331  /* reload and refresh the disk capacity */
#define HDIO_GET_NATIVE_MAX         0x0332  /* get the native max address for the device */
#define HDIO_SET_VISIBLE_MAX        0x0333  /* set the visible max address for the device */

/**
  * get_native_max() - Get the native max address of a disk.
  * @fd: open file descriptor.
  * @native_max: Pointer to hold the value
  *
  * Retrieves the native max address ofa disk.
  */
static int get_native_max( int fd, uint64_t *native_max )
{
	int result = 0;

	/* use our custom HDIO IOCTL to get the native capacity */
	/* NOTE: this wont work on stock kernel, this IOCTL is available
	 * only on kernels with zclone_dco patch; our custom zclone kernel.
	 */
	if ( 0 != ( result = ioctl( fd, HDIO_GET_NATIVE_MAX, native_max ) ) ) {
		int err = errno;
		printf("get_native_max: HDIO_GET_NATIVE_MAX Failed - %d\n", err );
	}

	if ( 0 == *native_max ) {
		printf("get_native_max: ignoring native_max=%llu\n", *native_max);
		result = -1;
		goto out;
	}

	printf("get_native_max: native_max=%llu\n", *native_max);
out:
	return result;
}

/**
  * get_visible_max() - Get the visible max address of a disk.
  * @fd: open file descriptor.
  * @visible_max: Pointer to hold the value
  *
  * Retrieves the visible max address ofa disk.
  */
static int get_visible_max( int fd, uint64_t *visible_max )
{
	int result;
	unsigned long long sz = 0;
	int sect_sz;

	if( 0 == ( result = ioctl(fd, BLKGETSIZE64, &sz) ) )
		if( 0 == ( result = ioctl(fd, BLKSSZGET, &sect_sz) ) )
			*visible_max = sz/sect_sz;

	printf("get_visible_max: sz=%llu, sect_sz=%d, visible_max=%llu\n",
			sz, sect_sz, *visible_max);

	return result;
}
int main(int argc, char **argv)
{
	int fd = 0;
	int result = -1;
	uint64_t nmax = 0;
	uint64_t vmax = 0;

	printf("set_disk_hpa: opening disk (%s)\n", argv[1]);
	if ( 0 >= ( fd = open( argv[1], O_RDONLY | O_LARGEFILE ) ) )
	{   
		printf("set_disk_hpa: opening disk (%s) failed!\n", argv[1]);
		goto cleanup;
	}

	/* use our custom HDIO IOCTL to refresh the disk capacity
	 * NOTE: this wont work on stock kernel, this IOCTL is available
	 * only on kernels with zclone_dco patch.
	 */
	if ( 0 != ( result = ioctl( fd, HDIO_REFRESH_DISK_CAPACITY, NULL ) ) ) {
	    int err = errno;
	    printf( "refresh_disk_capacity: HDIO_REFRESH_DISK_CAPACITY failed!; check dmesg - %d\n", err );
	}

	get_native_max(fd, &nmax);
	get_visible_max(fd, &vmax);

	close(fd);

cleanup:
	return 0;
}

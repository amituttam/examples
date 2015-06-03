#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>

#include <scsi/sg_lib.h>
#include <scsi/sg_io_linux.h>
#include <scsi/sg_cmds_basic.h>

#define uint64_t long long unsigned int

int main(int argc, char *argv[])
{
	int sg_fd;
	int ver;
	int rc = -1;
	int err = 0;
	unsigned char resp_buff[32];
	unsigned int last_blk_addr, block_size;
	uint64_t llast_blk_addr;

	/* N.B. An access mode of O_RDWR is required for some SCSI commands */
	if ( 0 >= ( sg_fd = open( argv[1], O_RDONLY | O_LARGEFILE ) ) ) {
		err = errno;
		printf( "disk_size_from_read_capacity (%s): failed to open disk (%d - %s)\n",
				argv[1], err, strerror(err));
		goto out;
	}

	/* Just to be safe, check we have a new sg device by trying an ioctl */
	if ( ( rc = ioctl( sg_fd, SG_GET_VERSION_NUM, &ver ) )  || ( ver < 30000 ) ) {
		err = errno;
		printf( "disk_size_from_read_capacity (%s): version number error (%d - %s)\n",
				argv[1], err, strerror(err));
		goto out;
	}

	/* READ CAPACITY */
	rc = sg_ll_readcap_10(sg_fd, 0, 0, resp_buff, 32, 0, 0);
	if (rc == 0)
	{
		printf("disk_size_from_read_capacity: READ CAPACITY (10) OK\n");
		last_blk_addr = ((resp_buff[0] << 24) | (resp_buff[1] << 16) |
		                (resp_buff[2] << 8) | resp_buff[3]);
		if (last_blk_addr != 0xffffffff)
		{
			block_size = ((resp_buff[4] << 24) | (resp_buff[5] << 16) |
			             (resp_buff[6] << 8) | resp_buff[7]);
			printf("READ CAPACITY (10): blocks: %u, blocksize: %d\n",
					last_blk_addr + 1, block_size);
		}
		else
		{
			printf("disk_size_from_read_capacity: Device capacity too large "
				   "trying 16 byte cdb variant\n");
			rc = sg_ll_readcap_16(sg_fd, 0, 0, resp_buff, 32, 0, 0);
			if (rc == 0)
			{
				printf("disk_size_from_read_capacity: READ CAPACITY (16) OK\n");

				/* Get last block addr */
				int k;
				for (k = 0, llast_blk_addr = 0; k < 8; ++k)
				{
					llast_blk_addr <<= 8;
					llast_blk_addr |= resp_buff[k];
				}

				/* Get block size */
				block_size = ((resp_buff[8] << 24) |
							  (resp_buff[9] << 16) |
							  (resp_buff[10] << 8) |
							  resp_buff[11]);

				/* Save results */
				printf("READ CAPACITY (16): blocks: %llu, blocksize: %d\n",
						llast_blk_addr + 1, block_size);
			}
			else
			{
				printf("disk_size_from_read_capacity: READ CAPACITY (16) FAILED\n");
				rc = -1;
			}
		}
	}
	else
	{
		printf("disk_size_from_read_capacity: READ CAPACITY (10) FAILED\n");
		rc = -1;
	}

out:
	if (sg_fd)
		close(sg_fd);

	return rc;
}

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
	int long_lba = 0;
	int offset, dev_specific_param;
	int calc_len, bd_len, bd_blk_len;
	unsigned char resp_buff[252];

	/* N.B. An access mode of O_RDWR is required for some SCSI commands */
	if ( 0 >= ( sg_fd = open( argv[1], O_RDONLY | O_LARGEFILE ) ) ) {
		err = errno;
		printf( "disk_size_from_mode_sense (%s): failed to open disk (%d - %s)\n",
				argv[1], err, strerror(err));
		goto out;
	}

	/* Just to be safe, check we have a new sg device by trying an ioctl */
	if ( ( rc = ioctl( sg_fd, SG_GET_VERSION_NUM, &ver ) )  || ( ver < 30000 ) ) {
		err = errno;
		printf( "disk_size_from_mode_sense (%s): version number error (%d - %s)\n",
				argv[1], err, strerror(err));
		goto out;
	}

	/* MODE SENSE (10) */
again_with_long_lba:
	printf("disk_size_from_mode_sense: Calling MODE SENSE\n");
	memset(resp_buff, 0, sizeof(resp_buff));
	rc = sg_ll_mode_sense10(sg_fd, long_lba, 0, 0, 1, 0, resp_buff, 252, 0, 0);
	if (rc == 0)
	{
		printf("disk_size_from_mode_sense: MODE SENSE OK\n");

		/* Fill in params */
		calc_len = (resp_buff[0] << 8) + resp_buff[1] + 2;
		dev_specific_param = resp_buff[3];
		bd_len = (resp_buff[6] << 8) + resp_buff[7];
		long_lba = (resp_buff[4] & 1);
		offset = 8;

		/* prepare for mode select */
		resp_buff[0] = 0;
		resp_buff[1] = 0;
		resp_buff[2] = 0;
		resp_buff[3] = 0;

		if ((offset + bd_len) < calc_len)
			resp_buff[offset + bd_len] &= 0x7f;  /* clear PS bit in mpage */

		bd_blk_len = 0;
		if (dev_specific_param & 0x40)
			printf("disk_size_from_mode_sense: Write Protect (WP) bit set\n");

		/* Compute number of blocks and block size */
		if (bd_len > 0)
		{
			/* Number of blocks */
			int j;
			uint64_t ull = 0;
			for (j = 0; j < (long_lba ? 8 : 4); ++j) {
				if (j > 0)
					ull <<= 8;
				ull |= resp_buff[offset + j];
			}

			printf("ULL: %x\n", ull);
			if ((0 == long_lba) && (0xffffffff == ull))
			{
				printf("disk_size_from_mode_sense: Capacity too large\n");
				printf("disk_size_from_mode_sense: Trying with Long LBA Mode\n");
				long_lba = 1;
				goto again_with_long_lba;
			}

			/* Block size */
			if (long_lba)
				bd_blk_len = (resp_buff[offset + 12] << 24) +
							 (resp_buff[offset + 13] << 16) +
							 (resp_buff[offset + 14] << 8) +
							 resp_buff[offset + 15];
			else
				bd_blk_len = (resp_buff[offset + 5] << 16) +
							 (resp_buff[offset + 6] << 8) +
							 resp_buff[offset + 7];

			/* Print */
			printf("MODE SENSE: blocks: %llu, blocksize: %d\n",
					ull, bd_blk_len);
		}
		else
		{
			printf("disk_size_from_mode_sense: No block descriptors present\n");
			rc = -1;
		}
	}
	else
	{
		printf("disk_size_from_mode_sense: MODE SENSE FAILED\n");
		rc = -1;
	}

out:
	if (sg_fd)
		close(sg_fd);

	return rc;
}

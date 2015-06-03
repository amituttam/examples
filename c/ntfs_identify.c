#include <tsk3/libtsk.h>
#include <tsk3/fs/tsk_ntfs.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fs.h>

#define tsk_getu32(endian, x)	\
(uint32_t)( ((endian) == TSK_LIT_ENDIAN)  ?	\
            ((((uint8_t *)(x))[0] <<  0) + \
             (((uint8_t *)(x))[1] <<  8) + \
             (((uint8_t *)(x))[2] << 16) + \
             (((uint8_t *)(x))[3] << 24) ) \
                                          :	\
            ((((uint8_t *)(x))[3] <<  0) + \
             (((uint8_t *)(x))[2] <<  8) + \
             (((uint8_t *)(x))[1] << 16) + \
             (((uint8_t *)(x))[0] << 24) ) )

static inline uint32_t ntfs_popcount32(uint32_t v)
{
	const uint32_t w = v - ((v >> 1) & 0x55555555);
	const uint32_t x = (w & 0x33333333) + ((w >> 2) & 0x33333333);
	return (((x + (x >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

static uint8_t
ntfs_get_num_free_clusters(NTFS_INFO *ntfs)
{
	int print = 1;
	int count = 0;
	uint32_t total = 0;

	char *ptr = ntfs->bmap_buf;
	int bits_p_clus = 8 * ntfs->csize_b;
	int len = ntfs->bmap->len * ntfs->csize_b; 
	int words = len / 4;

	printf("len: %d words: %d\n", len, words);

//	for (; len > 0; --len)
	int i;
	for (i = 0; i < words; i++)
	{
		/* Count number of set clusters */
		total += ntfs_popcount32(*ptr);

		/* Print */
//		if (print)
//		{
//			printf("%p: ", ptr);
//			printf("%08X\n", *ptr);
//		}
		*ptr++;
	}
	printf("number of set bits: %u\n", total);
}

int main(int argc, char *argv[])
{
	/* Image data */
	TSK_IMG_TYPE_ENUM imgtype = TSK_IMG_TYPE_DETECT;
	TSK_IMG_INFO *img;

	/* File system data */
	TSK_FS_TYPE_ENUM fstype = TSK_FS_TYPE_DETECT;
	TSK_FS_INFO *fs;

	/* Open the NTFS filesystem. This is opened as an image first and
	 * then the filesystem info is extracted.
	 */
	img = tsk_img_open_sing((const TSK_TCHAR *) argv[1], imgtype, 0);
	if (img == NULL)
	{
		tsk_error_print(stderr);
		exit(1);
	}

	/* Get the filesystem info */
	fs = tsk_fs_open_img(img, 0, fstype);
	if (fs == NULL)
	{
		tsk_error_print(stderr);
		img->close(img);
		exit(1);
	}

	/* NTFS */
	NTFS_INFO *ntfs = (NTFS_INFO *) fs;

	/* Dump FS information */
	printf("cluster size (bytes): %u\n", ntfs->csize_b);
	printf("sector size (bytes): %u\n", ntfs->ssize_b);
	printf("sectors per cluster: %u\n", ntfs->fs->csize);
	printf("size of volume (sectors): %u\n", tsk_getu32(ntfs->fs_info.endian, ntfs->fs->vol_size_s));
	printf("size of volume (clusters): %u\n", ntfs->fs_info.block_count);
	printf("size of volume (bytes): %llu\n", ntfs->fs_info.block_count * ntfs->csize_b);
	printf("first cluster: %llu\n", fs->first_block);
	printf("last cluster: %llu\n", fs->last_block);
	printf("address of last cluster: %llu\n", fs->last_block_act);
	printf("data unit type: %s\n", fs->duname);
	printf("bitmap len in blocks: %llu\n", ntfs->bmap->len);
	printf("bitmap len in bytes: %llu\n", ntfs->bmap->len * fs->block_size);

	/* BLKGETSIZE64 from kernel */
	int fd;
	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		printf("Could not open disk");
	else
	{
		unsigned long long sz;
		if (ioctl(fd, BLKGETSIZE64, &sz) != -1) 
			printf("BLKGETSIZE64: %llu\n", sz);
		else
			printf("error in getting BLKGETSIZE64\n");
		close(fd);
	}

	/* Number of free/used clusters */
	int i = 0;
	int used = 0;
	int free = 0;
	int unkn = 0;
	for (i = 0; i < ntfs->fs_info.block_count; i++)
	{
		TSK_FS_BLOCK_FLAG_ENUM clust_flag = ntfs->fs_info.block_getflags(&ntfs->fs_info, i);
		if (clust_flag == TSK_FS_BLOCK_FLAG_ALLOC)
			used++;
	}

	printf("free clusters: %d\n", ntfs->fs_info.block_count - used);
	printf("clusters used: %d\n", used);

	fs->fsstat(fs, stdout);

	/* Clean up */
	fs->close(fs);
	img->close(img);

	return 0;
}

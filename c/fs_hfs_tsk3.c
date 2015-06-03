#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/fs.h>

#include <tsk3/libtsk.h>
#include <tsk3/fs/tsk_hfs.h>

#define __STDC_FORMAT_MACROS

#define FS_IMG_STR_LEN          32
#define PED_STRING_LEN          128
#define FS_BM_DATA_TYPE_IN_BITS  ( sizeof( fs_bm_data_type ) * 8 )
#define FS_BM_DATA_TYPE_IN_BYTES ( sizeof( fs_bm_data_type ) )
#define FS_STRUCT_ALIGNMENT       ( 0x1000 )
#define FS_STRUCT_ALIGNMENT_MASK  ( ~( FS_STRUCT_ALIGNMENT - 1 ) )
#define FS_STRUCT_START_ALIGNMENT ( 512 )
#define FS_GET_ALIGNED_SIZE( SIZE ) ( ( ( SIZE ) + FS_STRUCT_ALIGNMENT - 1 ) & FS_STRUCT_ALIGNMENT_MASK )
#define FS_BITMAP_S_SIZE( BMSIZE ) FS_GET_ALIGNED_SIZE( ( sizeof( fs_bitmap_S ) + BMSIZE ) )

typedef unsigned long long fs_bm_data_type;

typedef enum fs_type {
	FS_TYPE_INVALID  = 0x0000000000000000,
	FS_TYPE_EXT      = 0x0000000000000001,
	FS_TYPE_HFS      = 0x0000000000000002,
	FS_TYPE_NTFS     = 0x0000000000000003,
	FS_TYPE_SWAP     = 0x0000000000000004,
	FS_TYPE_FAT      = 0x0000000000000005
} fs_type_E;

typedef struct fs_data
{
	fs_type_E                 type;
	char                type_str[ FS_IMG_STR_LEN ];
	char		    partn_name[ PED_STRING_LEN ];
	unsigned int        partno;
	/* TODO: Should we store partition type?
	 * And partition flags?
	 * */

	unsigned long long  blk_size;
	unsigned long long  blk_count;
	unsigned long long  blk_free;
	unsigned long long  blk_used;
	unsigned long long  fs_size;

	unsigned long long  bm_aligned_size;
	unsigned long long  bm_absolute_offset;
} fs_data_S;

typedef struct fs_bitmap
{
	unsigned long long  bm_bytes;
	unsigned long long  bm_pieces;
	unsigned long long  struct_align_size;
	unsigned long long  struct_alignment;
	unsigned long long  struct_chunks;
	fs_bm_data_type     bm[ 1 ];
} fs_bitmap_S;

typedef enum fs_ret {
	FS_SUCCESS,
	FS_GENERAL_ERROR,
	FS_OPEN_ERROR,
	FS_VOLUME_ERROR,
	FS_MEMORY_ERROR,
	FS_IDENTIFY_WITH_ERRORS,
	FS_IDENTIFY_UNCLEAN_MOUNT,
	FS_IDENTIFY_CHECK_NEEDED,
	FS_IDENTITY_UNSUPPORTED_VERSION,
	FS_IDENTITY_VOLUME_ERROR,
	FS_BITMAP_MISSING,
	FS_BITMAP_GROUP_ERROR,
	FS_BITMAP_ERROR,
	FS_BITMAP_MEMORY_ERROR,
	FS_IDENTIFY_FAILURE,
	FS_BOOTSECTOR_READ_ERROR,
	FS_ABORTED,
} fs_ret_E;

static inline fs_ret_E
fs_struct_fsbitmap_alloc( fs_bitmap_S **bm, unsigned int aligned_bm_size )
{
	if ( posix_memalign( ( void ** ) bm, FS_STRUCT_START_ALIGNMENT, aligned_bm_size ) )
		return FS_BITMAP_MEMORY_ERROR;

	memset( ( *bm ), 0, aligned_bm_size );

	return FS_SUCCESS;
}

static fs_ret_E
fs_hfs_get_size( const char* device, uint64_t *sz )
{
	int fd;
	fs_ret_E ret = FS_GENERAL_ERROR;

	/* BLKGETSIZE64 from kernel */
	fd = open(device, O_RDONLY);
	if (fd == -1)
		printf("fs_hfs_get_size: Could not open disk %s\n", device);
	else
	{
		if (ioctl(fd, BLKGETSIZE64, sz) != -1) 
		{
			printf("fs_hfs_get_size: BLKGETSIZE64: %llu\n", *sz);
			ret = FS_SUCCESS;
		}
		else
			printf("fs_hfs_get_size: Error in getting BLKGETSIZE64\n");
		close(fd);
	}

	return ret;
}

static TSK_WALK_RET_ENUM
fs_hfs_walk_cb(const TSK_FS_BLOCK *a_block, void *a_ptr)
{
	fs_data_S *fsinfo = (fs_data_S *) a_ptr;

	//printf("blk_free: %llu\n", fsinfo->blk_free++);
	fsinfo->blk_free++;

	return TSK_WALK_CONT;
}

static fs_ret_E
fs_hfs_read_bitmap( HFS_INFO *hfs, fs_data_S *fsinfo, fs_bitmap_S **bm )
{
	fs_ret_E result = FS_SUCCESS;
	uint64_t current_block;
	uint64_t used_blocks = 0;
	unsigned int bitmap_size = ( hfs->fs_info.block_count + 7 ) / 8;
	unsigned int aligned_bm_size = FS_BITMAP_S_SIZE( bitmap_size );

	if ( FS_SUCCESS != ( result = fs_struct_fsbitmap_alloc( bm, aligned_bm_size ) ) )
		return result;
	( *bm )->bm_bytes = bitmap_size;
	( *bm )->bm_pieces = ( bitmap_size + FS_BM_DATA_TYPE_IN_BYTES - 1 ) / FS_BM_DATA_TYPE_IN_BYTES;
	( *bm )->struct_align_size = aligned_bm_size;
	( *bm )->struct_alignment = FS_STRUCT_ALIGNMENT;
	( *bm )->struct_chunks = aligned_bm_size / FS_STRUCT_ALIGNMENT;

	for ( current_block = 0; current_block < hfs->fs_info.block_count; current_block++ )
	{
		TSK_FS_BLOCK_FLAG_ENUM bit = hfs->fs_info.block_getflags(&hfs->fs_info, current_block);
		unsigned int curr_byte_bm = current_block / FS_BM_DATA_TYPE_IN_BITS;
		unsigned int curr_bit_bm  = current_block % FS_BM_DATA_TYPE_IN_BITS;

		if ( bit & TSK_FS_BLOCK_FLAG_ALLOC )
		{
			( *bm )->bm[ curr_byte_bm ] |= ( fs_bm_data_type ) 1 << curr_bit_bm;
			++used_blocks;
		}
	}

	fsinfo->blk_used = used_blocks;
	fsinfo->blk_free = fsinfo->blk_count - used_blocks;
	printf("fs_hfs_read_bitmap: free: %llu used: %llu\n",
			fsinfo->blk_free, fsinfo->blk_used);

	return result;
}

fs_ret_E
fs_hfs_identify( const char *src, fs_data_S *inout, fs_bitmap_S **bm )
{
	uint64_t dev_sz;
	uint64_t vol_sz;
	fs_ret_E result;
	TSK_FS_INFO *fs;
	TSK_IMG_INFO *img;
	TSK_FS_TYPE_ENUM fstype = TSK_FS_TYPE_HFS_DETECT;
	TSK_IMG_TYPE_ENUM imgtype = TSK_IMG_TYPE_DETECT;

	/* Open the HFS filesystem. This is opened as an image first and
	 * then the filesystem info is extracted.
	 */
	img = tsk_img_open_sing((const TSK_TCHAR *) src, imgtype, 0);
	if (img == NULL)
	{
		printf("FS_OPEN_ERROR\n");
		return FS_OPEN_ERROR;
	}

	/* Get the filesystem info from the img */
	fs = tsk_fs_open_img(img, 0, fstype);
	if (fs == NULL)
	{
		printf("FS_IDENTIFY_FAILURE\n");
		img->close(img);
		return FS_IDENTIFY_FAILURE;
	}

	/* Device size */
	dev_sz = img->size;
	vol_sz = fs->block_count * fs->block_size;
	if ( dev_sz < vol_sz )
	{
		printf("FS_IDENTITY_VOLUME_ERROR\n");
		return FS_IDENTITY_VOLUME_ERROR;
	}
	printf("fs_hfs_identify: device size: %llu volume size: %llu\n",
			dev_sz, vol_sz);

	/* HFS */
	HFS_INFO *hfs = (HFS_INFO *) fs;

	/* General */
	inout->type = FS_TYPE_HFS;
	strcpy(inout->type_str, "HFS");

	/* Size */
	inout->blk_size = hfs->fs_info.block_size;
	inout->blk_count = hfs->fs_info.block_count;
	inout->fs_size = dev_sz;

	/* Bitmap */
	result = fs_hfs_read_bitmap( hfs, inout, bm );
	inout->bm_aligned_size = ( *bm )->struct_align_size;

	/* Clean up */
	fs->close(fs);
	img->close(img);

	return result;
}

int main(int argc, char *argv[])
{
	fs_data_S data;
	fs_bitmap_S *bitmap;

	printf("Identifying %s\n", argv[1]);
	if (fs_hfs_identify(argv[1], &data, &bitmap) == FS_SUCCESS)
		printf("successfully identified HFS drive %s\n", argv[1]);
	else
		printf("error\n");

	return 0;
}

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>

#include <ntfs/device.h>
#include <ntfs/volume.h>
#include <ntfs/bitmap.h>

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
	FS_TYPE_HFSP     = 0x0000000000000002,
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
fs_ntfs_open( const char* device, ntfs_volume **fspp )
{
	unsigned long long device_size, volume_size;

	if ( 0 == ( ( *fspp ) = ntfs_mount( device, NTFS_MNT_RDONLY ) ) )
		return FS_IDENTIFY_FAILURE;

	if ( ( *fspp )->flags & VOLUME_IS_DIRTY)
		// Attempt to sync it
		( *fspp )->dev->d_ops->sync( ( *fspp )->dev );

	/* Should  ->cluster_size be checked. What's the max cluster size?*/

	if ( ntfs_version_is_supported( ( *fspp ) ) )
		return FS_IDENTITY_UNSUPPORTED_VERSION;

	device_size = ntfs_device_size_get( ( *fspp )->dev, 1 );
	volume_size = ( *fspp )->nr_clusters * ( *fspp )->cluster_size;

	if ( device_size < volume_size )
		return FS_IDENTITY_VOLUME_ERROR;

	return FS_SUCCESS;
}

static fs_ret_E
fs_ntfs_read_bitmap( ntfs_volume *fs, fs_data_S *fsinfo, fs_bitmap_S **bm )
{
	fs_ret_E result = FS_SUCCESS;
	unsigned long long current_block;
	unsigned long long pos = 0;
	unsigned int bitmap_size = ( fs->nr_clusters + 7 ) / 8;
	unsigned int aligned_bm_size = FS_BITMAP_S_SIZE( bitmap_size );
	unsigned char *ntfs_bitmap = ( unsigned char * ) malloc( bitmap_size );
	unsigned int used_blocks = 0;
	unsigned int nonused_blocks = 0;

	if ( FS_SUCCESS != ( result = fs_struct_fsbitmap_alloc( bm, aligned_bm_size ) ) )
		return result;
	( *bm )->bm_bytes = bitmap_size;
	( *bm )->bm_pieces = ( bitmap_size + FS_BM_DATA_TYPE_IN_BYTES - 1 ) / FS_BM_DATA_TYPE_IN_BYTES;
	( *bm )->struct_align_size = aligned_bm_size;
	( *bm )->struct_alignment = FS_STRUCT_ALIGNMENT;
	( *bm )->struct_chunks = aligned_bm_size / FS_STRUCT_ALIGNMENT;

	ntfs_attr_pread( fs->lcnbmp_na, pos, bitmap_size, ntfs_bitmap );

	for ( current_block = 0; current_block < fs->nr_clusters; current_block++ )
	{
		unsigned char bit = ( unsigned char ) ntfs_bit_get( ntfs_bitmap, current_block );
		unsigned int curr_byte_bm = current_block / FS_BM_DATA_TYPE_IN_BITS;
		unsigned int curr_bit_bm  = current_block % FS_BM_DATA_TYPE_IN_BITS;

		if ( bit )
		{
			( *bm )->bm[ curr_byte_bm ] |= ( fs_bm_data_type ) 1 << curr_bit_bm;
			++used_blocks;
		}
		else
			++nonused_blocks;
	}

	free( ntfs_bitmap );

	/* At some time, try to figure out why fs->nr_free_clusters doesn't seem to
	 * reflect reality. For now overwrite fsinfo with values according to bitmap.
	 */
	fsinfo->blk_free = nonused_blocks;
	fsinfo->blk_used = used_blocks;
	printf("fs_ntfs_read_bitmap: free: %llu used: %llu\n",
			fsinfo->blk_free, fsinfo->blk_used);

	return result;
}

fs_ret_E
fs_ntfs_identify( const char *src, fs_data_S *inout, fs_bitmap_S **bm )
{
	fs_ret_E result;
	ntfs_volume *fs;

	if ( FS_SUCCESS == ( result = fs_ntfs_open( src, &fs ) ) )
	{
		inout->type = FS_TYPE_NTFS;
		strcpy( inout->type_str, "ntfs" );
		inout->blk_size = fs->cluster_size;
		inout->blk_count = fs->nr_clusters;
		inout->blk_free = fs->nr_free_clusters;
		inout->blk_used = fs->nr_clusters - fs->nr_free_clusters - 1;
		inout->fs_size = ntfs_device_size_get( fs->dev, 1 );
		result = fs_ntfs_read_bitmap( fs, inout, bm );
		inout->bm_aligned_size = ( *bm )->struct_align_size;

		ntfs_umount( fs, 0 );
	}
	else
		printf("Could not open ntfs drive %s\n", src);

	return result;
}

int main(int argc, char *argv[])
{
	fs_data_S data;
	fs_bitmap_S *bitmap;

	printf("Identifying %s\n", argv[1]);
	if (fs_ntfs_identify(argv[1], &data, &bitmap) == FS_SUCCESS)
		printf("successfully identified NTFS drive %s\n", argv[1]);
	else
		printf("error\n");

	return 0;
}

/*
 * fs_fat.c
 *
 *  Created on: Aug 12, 2009
 *      Author: hgunnarsson
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "fs_fat.h"

#define FS_IMG_STR_LEN          32
#define PED_STRING_LEN          128
#define FS_BM_DATA_TYPE_IN_BITS  ( sizeof( fs_bm_data_type ) * 8 )
#define FS_BM_DATA_TYPE_IN_BYTES ( sizeof( fs_bm_data_type ) )
#define FS_STRUCT_ALIGNMENT       ( 0x1000 )
#define FS_STRUCT_ALIGNMENT_MASK  ( ~( FS_STRUCT_ALIGNMENT - 1 ) )
#define FS_STRUCT_START_ALIGNMENT ( 512 )
#define FS_GET_ALIGNED_SIZE( SIZE ) ( ( ( SIZE ) + FS_STRUCT_ALIGNMENT - 1 ) & FS_STRUCT_ALIGNMENT_MASK )
#define FS_BITMAP_S_SIZE( BMSIZE ) FS_GET_ALIGNED_SIZE( ( sizeof( fs_bitmap_S ) + BMSIZE ) )
#define FS_SET_BM_BIT( BLOCK, BM_P )									\
{																		\
	unsigned int curr_byte_bm = BLOCK / FS_BM_DATA_TYPE_IN_BITS;		\
	unsigned int curr_bit_bm  = BLOCK % FS_BM_DATA_TYPE_IN_BITS;		\
	BM_P->bm[ curr_byte_bm ] |= ( fs_bm_data_type ) 1 << curr_bit_bm;	\
}

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

static int
get_fat_type( struct FatBootSector *fat_sb,
		char *fat_type,
		int *fat_num )
{
	int result = -1;

	/* TODO: make id more secure. Sig for 32, strcmp etc
	 * */
	if ( fat_sb->u.fat16.ext_signature == 0x29 )
	{
		if ( fat_sb->u.fat16.fat_name[4] == '6' )
		{
			*fat_num = FAT_16;
			strcpy( fat_type, "FAT16" );
			result = 0;
		}
		else if ( fat_sb->u.fat16.fat_name[4] == '2' )
		{
			*fat_num = FAT_12;
			strcpy( fat_type, "FAT12" );
			result = 0;
		}
	}
	else if ( fat_sb->u.fat32.fat_name[4] == '2' )
	{
		*fat_num = FAT_32;
		strcpy( fat_type, "FAT32" );
		result = 0;
	}

	return result;
}

static unsigned long long
get_total_sector( struct FatBootSector *fat_sb )
{
	unsigned long long total_sector = 0;

	if ( fat_sb->sectors != 0 )
		total_sector = ( unsigned long long ) fat_sb->sectors;
	else
		total_sector = ( unsigned long long ) fat_sb->sector_count;

	return total_sector;
}

static unsigned long long
get_sec_per_fat( struct FatBootSector *fat_sb )
{
	unsigned long long sec_per_fat = 0;

	if ( fat_sb->fat_length != 0 )
		sec_per_fat = fat_sb->fat_length;
	else
		sec_per_fat = fat_sb->u.fat32.fat_length;

	return sec_per_fat;
}

static unsigned long long
get_root_sec( struct FatBootSector *fat_sb )
{
	unsigned long long root_sec = ( ( fat_sb->dir_entries * 32 ) + fat_sb->sector_size - 1 ) /
			fat_sb->sector_size;

	return root_sec;
}

static unsigned long long
get_cluster_count( struct FatBootSector *fat_sb )
{
	unsigned long long total_sector  = get_total_sector( fat_sb );
	unsigned long long root_sec      = get_root_sec( fat_sb );
	unsigned long long sec_per_fat   = get_sec_per_fat( fat_sb );
	unsigned long long data_sec      = total_sector -
			( fat_sb->reserved + ( fat_sb->fats * sec_per_fat ) + root_sec );
	unsigned long long cluster_count = data_sec / fat_sb->cluster_size;

	return cluster_count;
}

static int
check_fat_status( int fatfd, int fatfs )
{
	int rd = 0;
	uint16_t Fat16_Entry;
	uint32_t Fat32_Entry;
	int fs_error = 2;
	int fs_good = 0;
	int fs_bad = 1;

	if (fatfs == FAT_16)
	{
		rd = read(fatfd, &Fat16_Entry, sizeof(Fat16_Entry));
        if (rd == -1)
            printf("error during read\n");
		rd = read(fatfd, &Fat16_Entry, sizeof(Fat16_Entry));
        if (rd == -1)
            printf("error during read\n");

		if ( !( Fat16_Entry & 0x8000 ) )
			return fs_bad;

		if ( !( Fat16_Entry & 0x4000 ) )
			return fs_error;
	}
	else if (fatfs == FAT_32)
	{
		rd = read(fatfd, &Fat32_Entry, sizeof(Fat32_Entry));
        if (rd == -1)
            printf("error during read\n");
		rd = read(fatfd, &Fat32_Entry, sizeof(Fat32_Entry));
        if (rd == -1)
            printf("error during read\n");

		if ( !( Fat32_Entry & 0x08000000 ) )
			return fs_bad;

		if ( !( Fat32_Entry & 0x04000000 ) )
			return fs_error;
	}
	else if (fatfs == FAT_12)
    {
		rd = read( fatfd, &Fat32_Entry, 3 );
        if (rd == -1)
            printf("error during read\n");
    }
	else
		return fs_error;

	return fs_good;
}

static unsigned long long
mark_reserved_sectors( struct FatBootSector *fat_sb,
		fs_bitmap_S *bm,
		unsigned long long block )
{
	unsigned long long i = 0;
	unsigned long long j = 0;
	unsigned long long sec_per_fat = get_sec_per_fat( fat_sb );
	unsigned long long root_sec = get_root_sec( fat_sb );

	for (i=0; i < fat_sb->reserved; i++,block++)
		FS_SET_BM_BIT( block, bm );

	for (j=0; j < fat_sb->fats; j++)
		for (i=0; i < sec_per_fat ; i++,block++)
			FS_SET_BM_BIT( block, bm );

	if (root_sec > 0) /// no rootdir sectors on FAT32
		for (i=0; i < root_sec; i++,block++)
			FS_SET_BM_BIT( block, bm );

	return block;
}

static fs_ret_E
check_fat32_entry( int fatfd,
		unsigned int csize,
		unsigned long long ccount,
		fs_bitmap_S *bm,
		unsigned long long *block,
		unsigned long long *bused )
{
	unsigned long long j, i;

	for ( i = 0; i < ccount; i++)
	{
		unsigned int fate;
		ssize_t rd = read(fatfd, &fate, sizeof( fate ) );
		if ( 4 != rd)
		{
            printf("fs_identify_with_errors\n");
			return FS_IDENTIFY_WITH_ERRORS;
		}

		if ( 0x00000000 != fate )
		{
			for ( j = 0; j < csize; j++, ( *block )++ )
				FS_SET_BM_BIT( ( *block ), bm );
			( *bused ) += csize;
		}
		else
			( *block ) += csize;

	}

	return FS_SUCCESS;
}

static fs_ret_E
check_fat16_entry( int fatfd,
		unsigned int csize,
		unsigned long long ccount,
		fs_bitmap_S *bm,
		unsigned long long *block,
		unsigned long long *bused )
{
	unsigned long long j, i;

	for ( i = 0; i < ccount; i++)
	{
		unsigned short fate;
		ssize_t rd = read(fatfd, &fate, sizeof( fate ) );
		if ( 2 != rd)
		{
            printf("fs_identify_with_errors\n");
			return FS_IDENTIFY_WITH_ERRORS;
		}

		if ( 0x0000 != fate )
		{
			for ( j = 0; j < csize; j++, ( *block )++ )
				FS_SET_BM_BIT( ( *block ), bm );
			( *bused ) += csize;
		}
		else
			( *block ) += csize;
	}

	return FS_SUCCESS;
}

static fs_ret_E
check_fat12_entry( int fatfd,
		unsigned int csize,
		unsigned long long ccount,
		fs_bitmap_S *bm,
		unsigned long long *block,
		unsigned long long *bused )
{
	unsigned long long j, i, c;
	unsigned long long two_reads = ccount / 2;
	unsigned long long one_read  = ccount % 2;

	for ( i = 0; i < ( two_reads + one_read ); i++)
	{
		unsigned char two_fates[ 3 ];
		unsigned short fates[ 2 ];
		unsigned int read_size = ( i < two_reads ) ? 3 : 2;
		ssize_t rd = read( fatfd, &two_fates[ 0 ], read_size );
		if ( read_size != rd)
		{
            printf("fs_identify_with_errors\n");
			return FS_IDENTIFY_WITH_ERRORS;
		}

		memcpy( &fates[ 0 ], &two_fates[ 0 ], sizeof( unsigned short ) );
		fates[ 0 ] &= 0x0FFF;
		if ( i < two_reads )
		{
			memcpy( &fates[ 1 ], &two_fates[ 1 ], sizeof( unsigned short ) );
			fates[ 1 ] >>= 4;
		}
		else
			memset( &fates[ 1 ], 0, sizeof( unsigned short ) );

		for ( c = 0; c < 2; c++ )
		{
			if ( 0x0000 != fates[ c ] )
			{
				for ( j = 0; j < csize; j++, ( *block )++ )
					FS_SET_BM_BIT( ( *block ), bm );
				( *bused ) += csize;
			}
			else
				( *block ) += csize;
		}
	}

	return FS_SUCCESS;
}

static fs_ret_E
fs_fat_open( const char* device,
		int *fatfd,
		struct FatBootSector *fat_sb,
		struct FatFsInfo *fatfs_info,
		char *fattype,
		int *fatnum )
{
	fs_ret_E result = FS_IDENTIFY_FAILURE;
	ssize_t sb_size;
	ssize_t info_size;

	if ( 0 >= ( ( *fatfd ) = open( device, O_RDONLY ) ) )
		return FS_IDENTIFY_FAILURE;

	if ( ( sizeof( *fat_sb ) == ( sb_size = read ( ( *fatfd ), fat_sb, sizeof( *fat_sb ) ) ) ) &&
			( sizeof( *fatfs_info ) == ( info_size = read( ( *fatfd ), fatfs_info, sizeof( *fatfs_info ) ) ) ) &&
			( 0 == get_fat_type( fat_sb, fattype, fatnum ) ) )
		result = FS_SUCCESS;

	return result;
}

static fs_ret_E
fs_fat_read_bitmap( int fatfd,
		int fatnum,
		struct FatBootSector *fat_sb,
		fs_data_S *inout,
		fs_bitmap_S **bm )
{
	fs_ret_E result;
	unsigned long long total_sector = get_total_sector( fat_sb );
	unsigned long long cluster_count = get_cluster_count( fat_sb );
	unsigned int bitmap_size = ( total_sector + 7 ) / 8;
	unsigned int aligned_bm_size = FS_BITMAP_S_SIZE( bitmap_size );
	unsigned long long block = 0;
	unsigned long long bused = 0;
	unsigned long long FatReservedBytes = fat_sb->sector_size * fat_sb->reserved;;

	if ( FS_SUCCESS != ( result = fs_struct_fsbitmap_alloc( bm, aligned_bm_size ) ) )
		return result;
	( *bm )->bm_bytes = bitmap_size;
	( *bm )->bm_pieces = ( bitmap_size + FS_BM_DATA_TYPE_IN_BYTES - 1 ) / FS_BM_DATA_TYPE_IN_BYTES;
	( *bm )->struct_align_size = aligned_bm_size;
	( *bm )->struct_alignment = FS_STRUCT_ALIGNMENT;
	( *bm )->struct_chunks = aligned_bm_size / FS_STRUCT_ALIGNMENT;

	bused = block = mark_reserved_sectors( fat_sb, ( *bm ), block);
	lseek( fatfd, FatReservedBytes, SEEK_SET );

	if ( 0 != check_fat_status( fatfd, fatnum ) )
	{
		free( *bm );
            printf("fs_identify_with_errors\n");
		return FS_IDENTIFY_WITH_ERRORS;
	}

	if (fatnum == FAT_16)
		result = check_fat16_entry( fatfd,
				fat_sb->cluster_size, cluster_count,
				( *bm ), &block, &bused );
	else if (fatnum == FAT_32)
		result = check_fat32_entry( fatfd,
				fat_sb->cluster_size, cluster_count,
				( *bm ), &block, &bused );
	else if (fatnum == FAT_12)
		result = check_fat12_entry( fatfd,
				fat_sb->cluster_size, cluster_count,
				( *bm ), &block, &bused );
	else
	{
            printf("fs_identify_with_errors\n");
		result = FS_IDENTIFY_WITH_ERRORS;
	}

	if ( FS_SUCCESS == result )
	{
		inout->blk_size = fat_sb->sector_size;
		inout->blk_count = total_sector;
		inout->blk_free = total_sector - bused;
		inout->blk_used = bused;
		inout->fs_size = total_sector * inout->blk_size;
		inout->bm_aligned_size = ( *bm )->struct_align_size;
	}
	else
		free( *bm );

	return result;
}

fs_ret_E
fs_fat_identify( const char *src, fs_data_S *inout, fs_bitmap_S **bm )
{
	fs_ret_E result;
	int fatfd;
	struct FatBootSector fat_sb;
	struct FatFsInfo     fatfs_info;
	char fattype[ 10 ];
	int fatnum = 0;

	memset( fattype, 0, sizeof( fattype ) );
	if ( FS_SUCCESS == ( result = fs_fat_open( src,
			&fatfd, &fat_sb, &fatfs_info, fattype, &fatnum ) ) )
	{
		inout->type = FS_TYPE_FAT;
		strcpy( inout->type_str, fattype );
		result = fs_fat_read_bitmap( fatfd, fatnum, &fat_sb, inout, bm );

		close( fatfd );
	}

	return result;
}

int main(int argc, char *argv[])
{
	fs_data_S data;
	fs_bitmap_S *bitmap;

	printf("Identifying %s\n", argv[1]);
	if (fs_fat_identify(argv[1], &data, &bitmap) == FS_SUCCESS)
		printf("successfully identified FAT drive %s\n", argv[1]);
	else
		printf("error\n");

	return 0;
}

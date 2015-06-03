#include <string.h>
#include <tsk/libtsk.h>
#include <tsk/fs/tsk_fs.h>
#include <tsk/fs/tsk_ntfs.h>

#define LOG_DEBUG(format, ...)          printf("%s==>%d " format "\n", __FILE__, __LINE__, ## __VA_ARGS__ )
#define LOG_INFO(format, ...)           printf("%s==>%d " format "\n", __FILE__, __LINE__, ## __VA_ARGS__ )
#define LOG_WARNING(format, ...)        printf("%s==>%d " format "\n", __FILE__, __LINE__, ## __VA_ARGS__ )
#define LOG_ERROR(format, ...)          printf("%s==>%d " format "\n", __FILE__, __LINE__, ## __VA_ARGS__ )

int
fsutil_validate_fs( const char *device, const char *fs_type )
{
	int rc = -1;
	TSK_FS_INFO *fs;
	TSK_IMG_INFO *img;
	TSK_FS_TYPE_ENUM fstype = 0;
	TSK_IMG_TYPE_ENUM imgtype = TSK_IMG_TYPE_DETECT;

	LOG_DEBUG("fsutil_validate_fs: device=%s fs_type=%s", device, fs_type);

	if (0 == strncmp( fs_type, "ntfs", 4 ))
		fstype = TSK_FS_TYPE_NTFS_DETECT;
	else if (0 == strncmp( fs_type, "ext", 3 ))
		fstype = TSK_FS_TYPE_EXT_DETECT;
	else {
		LOG_ERROR("fsutil_validate_fs: Unsuported fs(%s) on '%s'", fs_type, device);
		goto out;
	}

	/* Open the NTFS filesystem. This is opened as an image first and
	 * then the filesystem info is extracted.
	 */
	img = tsk_img_open_sing((const TSK_TCHAR *) device, imgtype, 0);
	if (img == NULL)
	{
		LOG_ERROR("fsutil_validate_fs: FS_OPEN_ERROR (%s) on '%s'",
			  fs_type, device);
		goto out;
	}

	/* Get the filesystem info from the img */
	fs = tsk_fs_open_img(img, 0, fstype);
	if (fs == NULL)
	{
		LOG_ERROR("fsutil_validate_fs: FS_IDENTIFY_FAILURE (%s) on '%s'",
			  fs_type, device);
		img->close(img);
		goto out;
	}

	LOG_DEBUG("fsutil_validate_fs: Validated fs=%s", tsk_fs_type_toname(fs->ftype));

	/* Clean up */
	fs->close(fs);
	img->close(img);
	rc = 0;

out:
	LOG_DEBUG("fsutil_validate_fs: DONE");
	return rc;

}

int main (int argc, char **argv)
{
	int rc = 0;
	LOG_DEBUG("main: argv[1]=%s, argv[2]=%s", argv[1], argv[2]);
	rc = fsutil_validate_fs(argv[1], argv[2]);
	LOG_DEBUG("main: rc=%d", rc);
	return 0;
}

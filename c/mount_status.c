#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#include <libmount.h>

int main (int argc, char *argv[])
{
	struct libmnt_fs *fs;
	struct libmnt_table *tab;

	mnt_init_debug(0xffff);

	tab = mnt_new_table_from_file("/proc/self/mounts");

	fs = mnt_table_find_target(tab, argv[2], MNT_ITER_BACKWARD);
	char *source = mnt_fs_get_source(fs);

	if (strcmp(source, argv[1]) == 0)
		printf("is mounted\n");
	else
		printf("could not find fs in tab\n");

	mnt_free_table(tab);

#if 0
	int rc;
	struct libmnt_fs *fs;
	struct libmnt_context *cxt = mnt_new_context();

	mnt_init_debug(0xffff);

	mnt_context_set_source(cxt, argv[1]);
	mnt_context_set_target(cxt, argv[2]);

	fs = mnt_context_get_fs(cxt);
	if (fs != NULL)
	{
		if (mnt_context_is_fs_mounted(cxt, fs, &rc))
			rc = 1;
		else
			printf("can't find fs in mtab\n");
	}
	else
		printf("Can't get fs from mnt context\n");

	mnt_free_context(cxt);
#endif

	return 0;
}

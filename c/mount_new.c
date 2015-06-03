#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#include <libmount.h>

char *
s_strdup(const char *str)
{
    if (!str)
        return NULL;
    return (char *)strdup(str);
}

int main(int argc, char *argv[])
{
	struct libmnt_table *tab;
	struct libmnt_fs *fs;
	char *test = NULL;

	if (test)
		printf("TEST IS NULL and TRUE\n");
	else
		printf("TEST IS NULL and FALSE\n");

	/* Enable debugging */
	//mnt_init_debug(0xffff);

	/* A mount table */
	tab = mnt_new_table_from_file("/proc/self/mounts");

	/* Get the root filesystem */
	fs = mnt_table_find_target(tab, "/", MNT_ITER_BACKWARD);

	const char *src = mnt_fs_get_source(fs);
	printf("root filesystem source: %s\n", src);

	char *root = mnt_pretty_path(src, NULL);
	printf("root filesystem source (pretty) %s\n", root);

	/* Save the root path */
	int i;
	for (i = 0; i < strlen(root); i++)
		if (isdigit(root[i]))
			root[i] = '\0';
	printf("disk_get_internal_repository: root drive found at '%s'\n", root);

	free(root);
	mnt_free_table(tab);
	printf("DONE\n");
	return 0;
}

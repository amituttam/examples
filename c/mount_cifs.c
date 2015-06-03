#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <libmount.h>

int main (int argc, char *argv[])
{
	struct libmnt_context *cxt;
	unsigned long flags;

	int fd = open("/tmp/sdsdsd", O_RDONLY);
	if (fd < 0)
		printf("file does not exists\n");
	else
	{
		printf("file exists\n");
		close(fd);
	}
	exit(0);

	mnt_init_debug(0xffff);

	cxt = mnt_new_context();
	//mnt_context_disable_helpers(cxt, 1);

	/* Params */
	flags = MS_NOEXEC | MS_NOSUID | MS_NODEV | MS_RDONLY;
	char *source = argv[1];
	char *target = argv[2];
	char *options = argv[3];
	
	/* Mount */
	mnt_context_set_source(cxt, source);
	mnt_context_set_target(cxt, target);
	mnt_context_set_options(cxt, options);
	mnt_context_set_mflags(cxt, flags);
	int rc = mnt_context_mount(cxt);
	printf("mount_cifs: mnt_context_mount return code = %d status = %d\n",
			rc, mnt_context_get_status(cxt));
	char error[128];
	mnt_context_strerror(cxt, error, 128);
	printf("mount_cifs: error: %s\n", error);

	mnt_free_context(cxt);

	return 0;
}

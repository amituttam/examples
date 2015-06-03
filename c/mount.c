#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mount.h>

int main(int argc, char *argv[])
{
	int res = 0;

	printf("mounting: %s to /mnt/usb\n", argv[1]);
	res = mount(argv[1], "/mnt/usb", "vfat", MS_RDONLY, "");
	if (res == -1)
		printf("%s (%d) \n", strerror(errno), errno);
	return 0;
}

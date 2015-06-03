#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	size_t size = 64;
	char destination[size];
	off_t offset = 0;
	int fd;

	if ((fd = open(argv[1], O_RDONLY)) == -1)
		return false;

	int l = lseek(fd, offset, SEEK_SET);
	if (l != -1)
	{
		int n = read(fd, destination, size);
		printf("n=%d\n", n);
		if (n != -1)
		{
			destination[n] = '\0';

			/* Remove trailing newline */
			printf("strlen(destination)=%d\n", strlen(destination));
			n = strlen(destination);
			printf("destination[n-1]=%s\n", destination[n-1]);
			if (destination[n-1] == '\n')
				destination[n-1] = '\0';
		}
	}

	(void)close(fd);

	printf("destination = %s", destination);

	return true;
}

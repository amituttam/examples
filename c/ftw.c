#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ftw.h>

static unsigned int total = 0;

int sum(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
	printf("fpath = %s\n", fpath);
	total += sb->st_size;
	return 0;
}

int main(int argc, char **argv) {

	int flags = FTW_PHYS | FTW_DEPTH;

	if (!argv[1] || access(argv[1], R_OK)) {
		return 1;
	}
	if (nftw(argv[1], &sum, 1, flags)) {
		perror("nftw");
		return 2;
	}
	printf("%s: %u\n", argv[1], total);
	return 0;
}

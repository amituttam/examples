#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <linux/fiemap.h>

#define LOG_DEBUG(format, ...)          printf("%s==>%d " format "\n", __FILE__, __LINE__, ## __VA_ARGS__ )
#define LOG_INFO(format, ...)           printf("%s==>%d " format "\n", __FILE__, __LINE__, ## __VA_ARGS__ )
#define LOG_WARNING(format, ...)        printf("%s==>%d " format "\n", __FILE__, __LINE__, ## __VA_ARGS__ )
#define LOG_ERROR(format, ...)          printf("%s==>%d " format "\n", __FILE__, __LINE__, ## __VA_ARGS__ )

#define MAX_FMAP_BLKS	256

int main(int argc, char**argv)
{
	int parse = 0;
	ssize_t read;
	size_t len = 0;
	char *line = NULL;
	struct fiemap *fmap = NULL;
	FILE *out = popen("ntfscluster -f -F test2/test /dev/sdg1", "r");

	if (out == NULL)
	{
		int err = errno;
		LOG_ERROR("main: Failed to open read pipe, err= %d - '%s'",
				err, strerror(err));
	}

	/* Initialize fmap */
	fmap = calloc( 1, sizeof( struct fiemap ) + ( 256 * sizeof( struct fiemap_extent ) ) );
	fmap->fm_start = 0;
	fmap->fm_length = 10737418240ULL;
	fmap->fm_extent_count = MAX_FMAP_BLKS;

	while ( ( read = getline( &line, &len, out ) ) != -1 )
	{
		if (strncmp(line, "Extent", 6) == 0)
		{
			parse = 1;
			continue;
		}

		if (parse)
		{
			printf("%s", line);

			int extent;
			uint64_t logical;
			uint64_t physical;
			uint64_t length;
			char *cextent;
			char *clogical;
			char *cphysical;
			char *clength;
			char *cp = NULL;
			const char delim[] = ":,";

			cp = strdup(line);
			cextent = strtok(cp, delim);     /* 0 */
			clogical = strtok(NULL, delim);  /* 0 */
			cphysical = strtok(NULL, delim); /* 12820480 */
			clength = strtok(NULL, delim);   /* 1238302720 */

			extent = atoi(cextent);
			logical = atoll(clogical);
			physical = atoll(cphysical);
			length = atoll(clength);

			LOG_DEBUG("extent=%d, logical=%lld, physical=%lld, length=%lld",
					extent, (long long) logical, (long long) physical, (long long) length);

			/* Insert into extent structure */
			fmap->fm_extents[extent].fe_logical = logical;
			fmap->fm_extents[extent].fe_physical = physical;
			fmap->fm_extents[extent].fe_length = length;
			fmap->fm_mapped_extents++;

			if (cp)
				free(cp);
		}
	}

	pclose(out);
	return 0;
}

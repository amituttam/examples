#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv)
{
	FILE *f;
	char line[1024];
	char *name = NULL;

	/* Open file for reading */
	f = fopen(argv[1], "r");
	if (f == NULL)
	{
		int err = errno;
		printf("Error opening file: %s (%d)\n", strerror(err), err);
		return 0;
	}

	/* Read until last line */
	while (fgets(line, sizeof(line), f) != NULL)
	{
		/* Do nothing, just loop until last line. */
	}
	printf("Last line: %s", line);

	/* Get initiator name */
	name = strchr(line, '=');
	if (name != NULL)
	{
		/* Remove trailing new line */
		size_t ln = strlen(name) - 1;
		if (name[ln] == '\n')
			name[ln] = '\0';
		printf("name=%s", name);
	}

	fclose(f);

	return 0;
}

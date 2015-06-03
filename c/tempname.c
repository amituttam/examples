#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	char tmpl[15];
	char *name;
	
	snprintf(tmpl, sizeof(tmpl), "swupdateXXXXXX");
	name = mktemp(tmpl);
	printf("name: %s\n", mktemp(tmpl));

	const char *events[] = {"activity", "success", "error"};
	int i;

	for (i = 0; i < 3; i++)
		printf("%s\n", events[i]);

#if 0
	int j = atoi(argv[1]);
	if (j)
		printf("j: %d is true\n", j);
	else
		printf("j: %d is false\n", j);
#endif

#if 0
	char *basec = strdup(argv[1]);
	char *bname = basename(basec);
	int len = strlen(bname);
	char *src = malloc(len-2);
	memcpy(src, bname, len-4);
	printf("src: %s\n", src);

	free(basec);
	free(src);
#endif

	const char *test = "TEST";
	if (test != NULL)
		printf("len(test): %d\n", strlen(test));


	return 0;
}

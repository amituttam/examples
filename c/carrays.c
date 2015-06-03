#include <stdio.h>
#include <string.h>

#define TOTAL 16

static char *name[TOTAL];

int main()
{
	int i;

	name[0] = strdup("TEST");

	for (i = 0; i < TOTAL; i++)
		printf("name[%d]: %s\n", i, name[i]);

	if (-1)
		printf("TRUE\n");

	const char *test = " ";

	if (test)
		printf("not empty\n");
	else
		printf("empty\n");

	test = NULL;
	if (test || *test)
		printf("test is not empty - %c\n", *test);
	else
		printf("test is NULL\n");
	return 0;
}

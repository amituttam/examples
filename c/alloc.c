#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	char *out;
	int i;

	//out = calloc(10, sizeof(char *));
	out = malloc(10 * sizeof(char *));

	printf("out: %s\n", out);

	return 0;
}

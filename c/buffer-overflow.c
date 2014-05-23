#include <string.h>
#include <stdio.h>

int main()
{
	char a[8];
	unsigned short b = 1979;

	strcpy(a, "1234567891011121314");

	printf("%s%d\n", a, b);
}

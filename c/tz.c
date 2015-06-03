#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int main ()
{
	tzset();

	printf("tzname[1]: %s tzname[2]: %s \n", tzname[0], tzname[1]);
	return 0;
}

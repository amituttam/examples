/* HTTP Request in C: https://gist.github.com/nolim1t/126991 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_SIZE	1024
#define HTTP_GET	"GET /\r\n"

int socket_connect(char *host, const char *port)
{
	int rc;
	int sfd;
	struct addrinfo hints;
	struct addrinfo *result, *rp;

	/* Get the address(es) for the matching host/port */

	/* Hints are criteria for selecting socket address structures
	 * returned in the list.
	 */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;      /* IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM;  /* Stream socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0;            /* Any protocol */

	/* Resolve the address */
	rc = getaddrinfo(host, port, &hints, &result);
	if (rc != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
		exit(EXIT_FAILURE);
	}

	/* Try each address returned by getaddrinfo() until we successfully
	 * connect.
	 */
	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		/* Error */
		if (sfd == -1)
		{
			printf("Opening socket failed for ai_family=%d. ai_socktype=%d, ai_protocol=%d\n",
					rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			continue;
		}

		/* Socket opened, now try and connect */
		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
		{
			printf("Successfully connected to socket for ai_family=%d. ai_socktype=%d, ai_protocol=%d\n",
					rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			break;
		}

		/* Clean up */
		close(sfd);
	}

	if (rp == NULL)
	{
		fprintf(stderr, "Could not connect\n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);

	return sfd;
}

int main(int argc, char *argv[])
{
	int fd;
	char buffer[BUFFER_SIZE];

	/* Check for number of arguments */
	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
		exit(1);
	}

	/* Open and connect to a socket */
	fd = socket_connect(argv[1], argv[2]);

	/* Send HTTTP GET command */
	int len = strlen(HTTP_GET);
	if (write(fd, HTTP_GET, len) != len)
	{
		fprintf(stderr, "partial/failed write\n");
		exit(EXIT_FAILURE);
	}

	/* Write zero valued bytes to clear buffer */
	bzero(buffer, BUFFER_SIZE);

	/* Response */
	while(read(fd, buffer, BUFFER_SIZE - 1) != 0)
	{
		fprintf(stderr, "%s", buffer);
		bzero(buffer, BUFFER_SIZE);
	}

	/* Clean up */
	shutdown(fd, SHUT_RDWR);
	close(fd);

	return 0;
}

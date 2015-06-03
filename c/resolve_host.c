#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

char *resolve_host(const char *path)
{
	int rc;
	int err;
	char *tmp;
	char *ip = NULL;
	char host[64] = {0};
	struct addrinfo *res, *ainfo;

	/* Check if the share path is valid and split the host name from the
	 * share name. Thus, //server/share -> server
	 */
	snprintf(host, 64, "%s", (path + 2));
	tmp = strchr(host,'/');
	if(!tmp)
	{
		printf("resolve_host: share path format invalid\n");
		goto out;
	}
	*tmp = '\0';
	printf("resolve_host: Hostname=<%s>\n", host);

	/* Resolve the address */
	rc = getaddrinfo(host, NULL, NULL, &res);
	if (rc != 0)
	{
		err = errno;
		printf("resolve_host: getaddrinfo failed %s\n", strerror(err));
		goto out;
	}

	/* Returns a list of address structures. Try each address until bind
	 * is successful.
	 */
	for(ainfo = res; ainfo != NULL; ainfo = ainfo->ai_next)
	{
		/* Ignore stream sockets or TCP addresses */
		if (ainfo->ai_socktype != SOCK_STREAM || ainfo->ai_protocol != IPPROTO_TCP)
		{
			ainfo = ainfo->ai_next;
			continue;
		}

		/* If the address family is IPv4, then save the address */
		if (AF_INET == ainfo->ai_addr->sa_family)
		{
			const char *ipaddr;
			char buffer[NI_MAXHOST + 1 + 10 + 1];
			struct sockaddr_in *sin = (struct sockaddr_in *) ainfo->ai_addr;

			/* Convert network address structure into a character string */
			ipaddr = inet_ntop(AF_INET, &sin->sin_addr, buffer, sizeof(buffer));
			if (ipaddr == NULL)
			{
				err = errno;
				printf("resolve_host: inet_ntop failed %s\n", strerror(err));
				goto out;
			}

			/* Save the address */
			ip = malloc(32);
			snprintf(ip, 32, "%s", buffer);
			break;
		}
	}
	freeaddrinfo( res );
out:
	return ip;
}

int main(int argc, char *argv[])
{
	char *ip = NULL;
	ip = resolve_host(argv[1]);
	if (ip != NULL)
	{
		printf("Successfully resolved '%s' to '%s'\n", argv[1], ip);
		free(ip);
	}
	return 0;
}

#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <errno.h>  
#include <string.h>  
#include <netdb.h>  
#include <sys/types.h>  
#include <netinet/in.h>  
#include <sys/socket.h>  
      
int main(int argc, char *argv[])  
{  
  int sockfd, port;  
  struct hostent *he;  
      
      
  if (argc != 2)  
    {  
      fprintf(stderr,"usage: client hostname\n");  
      exit(1);  
    }  
      
  if ((he=gethostbyname(argv[1])) == NULL)    // get the host info  
    {  
      perror("gethostbyname");  
      exit(1);  
    }  
  for(port=0; port<=1; port++)  
    {  
      struct sockaddr_in their_addr; // connector's address information  
      if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)  
	{  
	  perror("socket");  
	  exit(1);  
	}  
      
      their_addr.sin_family = AF_INET;    // host byte order  
      their_addr.sin_port = htons(port);  // short, network byte order  
      their_addr.sin_addr = *((struct in_addr *)he->h_addr);  
      memset(&(their_addr.sin_zero), '\0', 8);  // zero the rest of the struct  
      
      if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)  
	{  
	  close(sockfd);  
	}  
      else  
	{  
	  printf("%i open\n", port);  
	  close(sockfd);  
	}  
    }  
}  

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int connfd;
	struct sockaddr_in servaddr;
	char str[INET_ADDRSTRLEN];

	connfd = socket(AF_INET, SOCK_STREAM, 0);
	if (connfd < 0) {
        fprintf(stderr, "create socket failed!\n");
		exit(-1);
	}

	char host[20] = "img.pconline.com.cn";

	struct hostent *hptr;

	if ((hptr = gethostbyname(host)) == NULL) {
		fprintf(stderr, "get host by name error for host: %s: %s",
		   host, hstrerror(h_errno));
		exit(-1);
	}

    char **pptr;
	for (pptr = hptr->h_aliases; *pptr != NULL; pptr++)
		printf("\talias: %s\n", *pptr);

	switch(hptr->h_addrtype) {
	case AF_INET:
		pptr = hptr->h_addr_list;
		for (; *pptr != NULL; pptr++)
			printf("\taddress: %s\n", inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
			break;
	default:
	    fprintf(stderr, "unknown address type");
		break;
	}
}




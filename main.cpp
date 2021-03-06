#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <unistd.h>

using std::cout;
using std::endl;
using std::string;

#define SERV_PORT 80
#define MAXLINE 1024
#define SA struct sockaddr

bool analyUrl(char *url, char *host, char *othPath);

void doit(int connfd, char *host, char *othPath);

int main(int argc, char **argv)
{
    int connfd;
	struct sockaddr_in servaddr;
	char str[INET_ADDRSTRLEN];

	if (argc != 2) {
        fprintf(stderr, "usage: <program-name> <webpage address>");
		exit(-1);
	}

	connfd = socket(AF_INET, SOCK_STREAM, 0);
	if (connfd < 0) {
        fprintf(stderr, "create socket failed!\n");
		exit(-1);
	}

	char host[MAXLINE], othPath[MAXLINE];
	analyUrl(argv[1], host, othPath);

	struct hostent *hptr;

	if ((hptr = gethostbyname(host)) == NULL) {
		fprintf(stderr, "get host by name error for host: %s: %s",
		   host, hstrerror(h_errno));
		exit(-1);
	}

    /* 打印网站的别名 */
	char **pptr;
	//for (pptr = hptr->h_aliases; *pptr != NULL; pptr++)
	//	printf("\talias: %s\n", *pptr);

	switch(hptr->h_addrtype) {
	case AF_INET:
		pptr = hptr->h_addr_list;
		for (; *pptr != NULL; pptr++){
			bzero(&servaddr, sizeof(servaddr));
			servaddr.sin_family = hptr->h_addrtype;
			servaddr.sin_port = htons(SERV_PORT);
			memcpy(&servaddr.sin_addr, *pptr, sizeof(*pptr));
			if (connect(connfd, (SA *)&servaddr, sizeof(servaddr)) < 0) {
				printf("IP:%s链接失败!\n", inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
				continue;
			} 
			else {
				printf("IP:%s连接成功!\n", inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
				break;
			}
		}
			//printf("\taddress: %s\n", inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
			break;
	default:
	    fprintf(stderr, "unknown address type");
		break;
	}

    doit(connfd, host, othPath);
}

bool analyUrl(char *url, char *host, char *othPath)
{
	char *pos = strstr(url, "http://");
	if (pos == NULL)
		return false;
	else
		pos += 7;

	sscanf(pos, "%[^/]%s", host, othPath);
	cout << "host: " << host << " othpath:" << othPath << endl;
	return true;
}


void doit(int connfd, char *host, char *othPath)
{
    string reqInfo = "GET " + (string)othPath + " HTTP/1.1\r\nHost: " + (string)host + "\r\nConnection:Close\r\n\r\n";
	cout << reqInfo << " " << sizeof(reqInfo.c_str()) << endl;

	int nwrite;
    nwrite = write(connfd, reqInfo.c_str(), reqInfo.size());

    std::cout << nwrite << endl;
    char buff[MAXLINE + 1];
	ssize_t n;
	while ((n = read(connfd, buff, MAXLINE)) > 0) {
		buff[n] = 0;
		if (fputs(buff, stdout) == EOF) {
			fprintf(stderr, "fputs error");
		}
	}
	if (n < 0) {
		fprintf(stderr, "read error");
		exit(-1);
	}
	exit(0);
}

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

using std::cout;
using std::endl;
using std::string;
using std::fstream;
using std::ios;

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
		fprintf(stderr, "usage: <program-name> <url address>");
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
		fprintf(stderr, "get host by name error for host: %s: %s", host,
		    hstrerror(h_errno));
		exit(-1);
	}

	char **pptr;

	switch(hptr->h_addrtype) {
	case AF_INET:
	    pptr = hptr->h_addr_list;
		for (; *pptr != NULL; ++pptr) {
			bzero(&servaddr, sizeof(servaddr));
			servaddr.sin_family = hptr->h_addrtype;
			servaddr.sin_port = htons(SERV_PORT);
			memcpy(&servaddr.sin_addr, *pptr, sizeof(*pptr));
			if (connect(connfd, (SA*)&servaddr, sizeof(servaddr)) < 0) {
				printf("IP: %s 连接失败!\n", inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
				continue;
			}
			else {
				printf("IP: %s 连接成功!\n", inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
				break;
			}
		}
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
	cout << "host: " << host << " othPath: " << othPath << endl;
	return true;
}

void doit(int connfd, char *host, char *othPath)
{
	string reqInfo = "GET " + (string)othPath + " HTTP/1.1\r\nHost: " + (string)host + "\r\nConnection:Close\r\n\r\n";
	
	write(connfd, reqInfo.c_str(), reqInfo.size());

    fstream picture;
	picture.open("pic.jpeg", ios::out | ios::binary);

	char buff[MAXLINE + 1];
	ssize_t n;
	
	n = read(connfd, buff, MAXLINE);
	char *cpos = strstr(buff, "\r\n\r\n");

	picture.write(cpos + strlen("\r\n\r\n"), n - (cpos - buff) - strlen("\r\n\r\n"));
	while ((n = read(connfd, buff, MAXLINE - 1)) > 0) {
		picture.write(buff, n);
	}
	picture.close();
}


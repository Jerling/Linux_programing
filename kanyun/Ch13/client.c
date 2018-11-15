#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define MAXLINE 1024

int tcp_connect(const char *host, const char *serv);

int main(int argc, char **argv)
{
    int     sockfd, n;
    char    recvline[MAXLINE + 1];
    socklen_t   len;
    struct  sockaddr_in cliaddr;

    if (argc != 3){
        printf("argument should be 3\n");
        exit(1);
    }
    sockfd = tcp_connect(argv[1], argv[2]);

    len = sizeof(cliaddr);
    getpeername(sockfd, (struct sockaddr *)&cliaddr, &len);
    inet_ntop(AF_INET, &cliaddr.sin_addr, recvline, sizeof(recvline));
    printf("connect to %s\n", recvline);

    while ((n = read(sockfd, recvline, MAXLINE)) > 0){
        recvline[n] = 0;
        fputs(recvline, stdout);
    }

    exit(0);
}

int tcp_connect(const char *host, const char *serv)
{
    int     sockfd, n;
    struct  addrinfo hints, *res, *ressave;
    struct  sockaddr_in *cliaddr;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((n = getaddrinfo(host, serv, &hints, &res)) != 0){
        printf("tcp_connect error for %s,%s:%s\n", host, serv, gai_strerror(n));
        exit(1);
    }

    ressave = res;
    do{
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0)
            continue;
        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
            break;

        cliaddr = (struct sockaddr_in *)res->ai_addr;
        close(sockfd);
    } while ((res = res->ai_next) != NULL);

    if (res == NULL)
        printf("tcp_connect error for %s,%s\n", host, serv);

    freeaddrinfo(ressave);

    return sockfd;
}

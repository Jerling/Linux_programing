#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLINE 1024
#define SA struct sockaddr

int main(int argc, char **argv)
{
    struct sockaddr_in  cliaddr;
    int     sockfd, n, opt;
    char    mesg[MAXLINE + 1];

    if (argc != 2){
        printf("usage:%s<port>\n", argv[1]);
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sin_port = htons(atoi(argv[1]));
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    opt = SO_REUSEADDR;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    bind(sockfd, (SA *)&cliaddr, sizeof(cliaddr));

    n = read(sockfd, mesg, MAXLINE);
    if (n > 0){
        mesg[n] = 0;
        printf("%s\n", mesg);
    }

    return 0;
}

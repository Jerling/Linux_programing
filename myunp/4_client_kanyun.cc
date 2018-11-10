#include "unp.h"

int main(int argc, char **argv)
{
    int     sockfd, n;
    struct  sockaddr_in servaddr;
    char    buff[MAXLINE + 1];
    struct  sockaddr_in cliaddr;
    socklen_t   cliLen;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERPORT);

    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    connect(sockfd, (SA *)&servaddr, sizeof(servaddr));
    cliLen = sizeof(cliaddr);
    getsockname(sockfd, (SA *)&cliaddr, &cliLen);

    while ((n = read(sockfd, buff, MAXLINE)) > 0){
        buff[n] = '\0';
        fputs(buff, stdout);
    }

    return 0;
}

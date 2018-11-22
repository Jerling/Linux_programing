#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 1024
#define SA struct sockaddr

int main(int argc, char **argv)
{
    struct sockaddr_in  cliaddr;
    int     sockfd, n, len;
    char    mesg[MAXLINE];
    time_t  ticks;

    if (argc != 2){
        printf("usage:%s<port>\n", argv[0]);
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sin_port = htons(atoi(argv[1]));
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    cliaddr.sin_family = AF_INET;

    bind(sockfd, (SA *)&cliaddr, sizeof(cliaddr));

    for ( ; ; ){
        len = sizeof(cliaddr);
        n = recvfrom(sockfd, mesg, MAXLINE, 0, (SA *)&cliaddr, &len);
        if (n < 0){
//          sleep(5);
            continue;
        }
        mesg[n] = 0;
        printf("recv: %s\n", mesg);
        ticks = time(NULL);
        snprintf(mesg, sizeof(mesg), "%.24s", ctime(&ticks));
        sendto(sockfd, mesg, 2424, 0, (SA *)&cliaddr, len);
    }

    return 0;
}

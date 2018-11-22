#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 1024
#define SA struct sockaddr

int main(int argc, char **argv)
{
    struct sockaddr_in  srvaddr;
    int     sockfd, on = 1;
    time_t  ticks;
    char    mesg[MAXLINE + 1];

    if (argc != 3){
        printf("usage:%s<ip address><port>\n", argv[0]);
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(int));
    bzero(&srvaddr, sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, argv[1], &srvaddr.sin_addr) <= 0){
        printf("wrong dest ip address\n");
        exit(0);
    }
    srvaddr.sin_port = htons(atoi(argv[2]));
    for ( ; ; ){
        ticks = time(NULL);
        snprintf(mesg, sizeof(mesg), "%.24s\r\n", ctime(&ticks));
        sendto(sockfd, mesg, strlen(mesg), 0, (SA *)&srvaddr, sizeof(srvaddr));
        sleep(5);
    }

    return 0;
}

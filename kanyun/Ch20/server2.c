#include <sys/socket.h>
#include <signal.h>
#include <stdio.h>
#include <netinet/in.h>
#include <time.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

extern int errno;
#define MAXLINE 1024
#define SA struct sockaddr

static void recvfrom_alarm(int signo);
void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen);

int main(int argc, char **argv)
{
    struct sockaddr_in      srvaddr;
    int     sockfd;

    if (argc != 3){
        printf("usage:%s<ip addr><port>\n", argv[0]);
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&srvaddr,sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &srvaddr.sin_addr);
    srvaddr.sin_port = htons(atoi(argv[2]));

    dg_cli(stdin, sockfd, (SA *)&srvaddr, sizeof(srvaddr));

    return 0;
}

static void recvfrom_alarm(int signo)
{
    return;
}
void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int     n;
    const   int on = 1;
    char    sendline[MAXLINE], recvline[MAXLINE + 1];
    socklen_t   len;
    struct sockaddr *preply_addr;

    preply_addr = malloc(servlen);

    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

    signal(SIGALRM, recvfrom_alarm);
    while (fgets(sendline, MAXLINE, fp) != NULL){
        sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

//      alarm(3);
//      for ( ; ; ){
        len = servlen;
        n = recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
        if (n < 0){
            if (errno == EINTR)
                break;
            else
                printf("recvfrom error\n");
        } else{
            recvline[n] = 0;
            inet_ntop(AF_INET, &preply_addr, sendline, sizeof(sendline));
            printf("from %s:%s\n", sendline, recvline);
        }
//      }
    }

    free(preply_addr);
}

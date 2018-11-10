#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>    // bzero
#include <arpa/inet.h> // inet_pton
#include <unistd.h>    // write read

#define MAXLINE 1024
#define SA struct sockaddr

void str_cli(FILE *fp, int sockfd);
int main(int argc, char **argv)
{
    int     sockfd[5];
    struct  sockaddr_in servaddr;
    int     i;
    for (i = 0; i < 5; i++){
        sockfd[i] = socket(AF_INET, SOCK_STREAM, 0);

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(9877);

        inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

        connect(sockfd[i], (SA *)&servaddr, sizeof(servaddr));
    }

    str_cli(stdin, sockfd[0]);
    return 0;
}
void str_cli(FILE *fp, int sockfd)
{
    char        sendline[MAXLINE], recvline[MAXLINE];

    while (fgets(sendline, MAXLINE, fp) != NULL){
        write(sockfd, sendline, strlen(sendline));
        if (read(sockfd, recvline, MAXLINE) == 0){
            printf("str_cli:server terminated prematurely\n");
            return;
        }
        fputs(recvline, stdout);
    }
}

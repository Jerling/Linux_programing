#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SA struct sockaddr
#define UNIXSTR_PATH "/tmp/unix.str"
#define MAXLINE 1024
void str_cli(FILE *fd, int sockfd);
int main(int argc, char **argv)
{
    int     sockfd;
    struct  sockaddr_un servaddr;

    sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXSTR_PATH);

    connect(sockfd, (SA *)&servaddr, sizeof(servaddr));

    str_cli(stdin, sockfd);

    exit(0);
}
void str_cli(FILE *fd, int sockfd)
{
    int     n;
    int     recvline[MAXLINE], sendline[MAXLINE];
    while (fgets(sendline, MAXLINE, fd) != NULL){
        write(sockfd, sendline, strlen(sendline));
        if ((n = read(sockfd, recvline, MAXLINE)) > 0){
            fputs(recvline, stdout);
        }
    }
}

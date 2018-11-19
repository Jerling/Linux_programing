#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#define SA struct sockaddr
#define MAXLINE 1024
#define UNIXSTR_PATH "/tmp/unix.str"
extern int errno;
void sig_chld(int);
void str_echo(int sockfd);

int main(int argc, char **argv)
{
    int     listenfd, connfd;
    pid_t   childpid;
    socklen_t   clilen;
    struct  sockaddr_un cliaddr, servaddr;

    listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);

    unlink(UNIXSTR_PATH);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXSTR_PATH);

    bind(listenfd, (SA *)&servaddr, sizeof(servaddr));
    listen(listenfd, 5);

    signal(SIGCHLD, sig_chld);

    for ( ; ; ){
        clilen = sizeof(cliaddr);
        if ((connfd = accept(listenfd, (SA *)&cliaddr, &clilen)) < 0){
            if (errno == EINTR)
                continue;
            else{
                printf("accept error\n");
                exit(0);
            }
        }

        if ((childpid = fork()) == 0){
            close(listenfd);
            str_echo(connfd);
            exit(0);
        }

        close(connfd);
    }
}

void str_echo(int sockfd)
{
    char        recvline[MAXLINE];
    int         n;

    while ((n = read(sockfd, recvline, MAXLINE)) > 0){
        recvline[n] = '\0';
        write(sockfd, recvline, n);
    }
}

void sig_chld(int signo)
{
    pid_t   pid;
    int     stat;

    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    return;
}

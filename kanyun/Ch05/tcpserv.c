#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <signal.h>
#include <errno.h>
#include <string.h> // for bzero
#include <unistd.h> // for fork
#include <wait.h>   // for waitpid

#define MAXLINE 1024
#define SA struct sockaddr
void sig_chld(int signo);
typedef void Sigfunc(int);
Sigfunc *Signal(int signo, Sigfunc *func);
void str_echo(int sockfd);

int main(int argc, char **argv)
{
    int     listenfd, connfd;
    pid_t   childpid;
    socklen_t   clilen;
    struct  sockaddr_in servaddr, cliaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9877);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

    listen(listenfd, 5);

    Signal(SIGCHLD, sig_chld);

    for ( ; ; ){
        clilen = sizeof(cliaddr);
        if ((connfd = accept(listenfd, (SA *)&cliaddr, &clilen)) < 0){
            if (errno == EINTR)
                continue;
            else{
                printf("accept error\n");
                exit(-1);
            }
        }
        if ((childpid = fork()) == 0){
            close(listenfd);
            str_echo(connfd);
            exit(0);
        }

        close(connfd);
    }

    return 0;
}
void sig_chld(int signo)
{
    pid_t   pid;
    int     stat;

    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);

    return;
}
Sigfunc *Signal(int signo, Sigfunc *func)
{
    struct  sigaction act, oact;
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if (signo == SIGALRM){
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
#endif
    } else {
        #ifdef SA_RESTART
        act.sa_flags |= SA_RESTART;
#endif
    }

    if (sigaction(signo, &act, &oact) < 0)
        return (SIG_ERR);

    return (oact.sa_handler);
}
void str_echo(int sockfd)
{
    char        buff[MAXLINE];
    int         n;

    for ( ; ; ){
        if ((n = read(sockfd, buff, MAXLINE)) > 0){
            buff[n] = '\0';
            write(sockfd, buff, n);
        }
        else if (n < 0 && errno == EINTR)
            continue;
        else if (n < 0){
            printf("str_echo:read error\n");
            return;
        }
        else if (n == 0){
            break;
        }
    }
}

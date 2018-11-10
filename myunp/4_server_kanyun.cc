#include "unp.h"
#include <iostream>

int main(int argc, char **argv)
{
    int     listenfd, connfd;
    char     buff[MAXLINE];
    pid_t   pid;
    time_t  ticks;
    struct  sockaddr_in servaddr;
    struct  sockaddr_in cliaddr;
    socklen_t   cliLen;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(9877);

    bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

    listen(listenfd, 5);

    for ( ; ; ){
        cliLen = sizeof(cliaddr);
        connfd = accept(listenfd, (SA *)&cliaddr, &cliLen);
        if ((pid = fork()) == 0){
            close(listenfd);
            ticks = time(NULL);
            snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
            std::cout << buff << "\n";
            write(connfd, buff, strlen(buff));
            _exit(0);
        }
        if (waitpid(pid, NULL, 0) != pid){
            printf("waitpid error\n");
            exit(1);
        }
        close(connfd);
    }

    return 0;
}

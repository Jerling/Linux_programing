#include "unp.h"
#include <iostream>

int main()
{
    int     listenfd, connfd;
    char    buff[MAXLINE];
    pid_t   pid_child;
    time_t  ticks;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;
    socklen_t cliLen;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    // listenfd = socket(AF_INET, SOCK_DGRAM, 0);

    addr_init(&servaddr, SERPORT, SERHOST);
    bind(listenfd, (SA*)&servaddr, sizeof(servaddr));

    listen(listenfd, 5);

    while (true) {
        cliLen = sizeof(cliaddr);
        connfd = accept(listenfd, (SA*)&cliaddr, &cliLen);
        if ((pid_child = fork()) == 0) {
            close(listenfd);
            ticks = time(NULL);
            snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
            std::cout << buff << "\n";
            write(connfd, buff, strlen(buff));
            _exit(0);
        }
        if (waitpid(pid_child, NULL, 0) != pid_child) {
            std::cout << strerror(errno) << "\n";
            exit(0);
        }
        close(connfd);
    }

    return 0;
}

#include "unp.h"

int main(int args, char *argv[])
{
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    addr_init(&address, SERPORT, SERHOST);

    bind(listenfd, (SA*)&address, sizeof(address));
    listen(listenfd, LISTENQ);
    printf("%s\n", "listening");

    while (true) {
        struct sockaddr_in cliaddr;
        socklen_t clilen;
        auto connfd = accept(listenfd, (SA*)&cliaddr, &clilen);
        printf("connfd : %d\n", connfd);
        pid_t pid_child;
        if ((pid_child = fork()) == 0) {
            close(listenfd);
            str_echo(connfd);
            exit(0);
        }
        if (waitpid(pid_child, nullptr, 0) != 0) {
            printf("%s\n", "waitpid error");
            exit(-1);
        }
        close(connfd);
    }
    return 0;
}

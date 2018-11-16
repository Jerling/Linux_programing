#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <time.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>

extern int errno;
int daemon_proc;

#define MAXLINE 1024
#define MAXFD 64

int daemon_init(const char *pname, int facility);
int tcp_listen(const char *host, const char *serv, socklen_t *addrlenp);

int main(int argc, char **argv)
{
    int     listenfd, connfd;
    socklen_t   len;
    char    buff[MAXLINE];
    time_t  ticks;
    struct  sockaddr_in cliaddr;

    daemon_init(argv[0], 0);

    listenfd = tcp_listen(argv[1], argv[2], NULL);

    for (; ;){
        len = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);
        inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff));
        strcat(buff, ".this is a test\n");
        syslog(LOG_INFO, "%s", buff);

        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        write(connfd, buff, strlen(buff));

        close(connfd);
    }
}

int daemon_init(const char *pname, int facility)
{
    int     i;
    pid_t   pid;
    if ((pid = fork()) < 0)
        return -1;
    else if (pid)
        _exit(0);

    if (setsid() < 0)
        return -1;
    signal(SIGHUP, SIG_IGN);
    if ((pid = fork()) < 0)
        return -1;
    else if (pid)
        _exit(0);

    daemon_proc = 1;

    chdir("/");
    for (i = 0; i < MAXFD; i++)
        close(i);

    open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);

    openlog(pname, LOG_PID, facility);
    return 0;
}

int tcp_listen(const char *host, const char *serv, socklen_t *addrlenp)
{
    int     listenfd, n;
    const int on = 1;
    struct  addrinfo hints, *res, *ressave;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((n = getaddrinfo(host, serv, &hints, &res)) != 0){
        printf("tcp_listen error for %s,%s:%s\n", host, serv, gai_strerror(n));
        exit(1);
    }

    ressave = res;

    do{
        listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (listenfd < 0)
            continue;
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
            break;
        close(listenfd);
    } while ((res = res->ai_next) != NULL);

    if (res == NULL)
        printf("tcp_listen error for %s,%s\n", host, serv);

    listen(listenfd, 5);
    if (addrlenp)
        *addrlenp = res->ai_addrlen;

    freeaddrinfo(ressave);

    return listenfd;
}

#include <sys/types.h>
#include <sys/socket.h>
#include <wait.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <utility>

#define MAXLINE 1024
#define LISTENQ 1024
#define SERPORT 8089
#define SERHOST INADDR_ANY
#define SA struct sockaddr

/**
struct in_addr{
  in_addr_t       s_addr;
};

struct sockaddr_in{
  uint8_t         sin_len;
  sa_family_t     sin_family;
  in_port_t       sin_port;
  struct in_addr  sin_addr;
  char            sin_zero[8];
};
**/

inline void addr_init(struct sockaddr_in *addr, const int af, const int port, const int ip=-1) {
  bzero(addr, sizeof(addr));
  addr->sin_family = af;
  addr->sin_port = htons(port);
  if (ip >= 0)
      addr->sin_addr.s_addr = htonl(ip);
}

/** 字节流套接字上调用read或write输入或输出的字节数可能比请求的数量少，
 * 然而这不是出错的状态。这个现象的原因在于内核中用于套接字的缓冲区可能已达到极限，
 * 此时所需的是调用者再次调用read或write函数，以输入或输出剩余的字节。
 */
ssize_t readn(int fd, void *vptr, size_t n)
{
    size_t      nleft;
    ssize_t     nread;
    char        *ptr;

    ptr = (char*)vptr;
    nleft = n;
    while (nleft > 0){
        if ((nread = read(fd, ptr, nleft)) < 0){
            if (errno == EINTR)
                nread = 0;
            else
                return (-1);
        } else if (nread == 0)
            break;
        nleft -= nread;
        ptr += nread;
    }

    return (n - nleft);
}

ssize_t writen(int fd, const void *vptr, size_t n)
{
    size_t      nleft;
    ssize_t     nwritten;
    const char *ptr;

    ptr = (char*)vptr;
    nleft = n;
    while (nleft > 0){
        if ((nwritten = write(fd, ptr, nleft)) <= 0){
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;
            else
                return (-1);
        }
        nleft -= nwritten;
        ptr += nwritten;
    }

    return (n);
}

void str_echo(int sockfd) {
    ssize_t n;
    char buff[MAXLINE];
again:
    while ((n = read(sockfd, buff, MAXLINE)) > 0){
        buff[n] = '\0';
        printf("received form client is %s\n", buff);
        write(sockfd, buff, n);
    }
    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0)
        printf("str_echo:read error\n");
}

void str_cli(FILE *fp, int sockfd)
{
    char  sendline[MAXLINE], recvline[MAXLINE];

    while (fgets(sendline, MAXLINE, fp) != nullptr){
        write(sockfd, sendline, strlen(sendline));
        if (read(sockfd, recvline, MAXLINE) == 0){
            printf("str_cli:server terminated prematurely\n");
            return;
        }
        fputs(recvline, stdout);
    }
}

#include "unp.h"

#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <iostream>

static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

namespace unp{
    static ssize_t my_read(int fd, char *ptr) {
        if(read_cnt <= 0) {
        again:
            if ((read_cnt = read(fd, read_buf, sizeof(read_buf))<0)) {
                if(errno == EINTR)
                    goto again;
                return(-1);
            }else if(read_cnt == 0) return(0);
            read_ptr = read_buf;
        }
        read_cnt--;
        *ptr = *read_ptr++;
        return (1);
    }

    ssize_t readline(int fd, void *vptr, size_t maxlen)
    {
        ssize_t n, rc;
        char c, *ptr;
        ptr = (char*)&vptr;
        for(n=1; n<maxlen; n++)
        {
            if((rc = my_read(fd, &c)) == 1)
            {
                *ptr++ = c;
                if(c == '\n')
                    break;
            }else if(rc == 0)
            {
                *ptr = 0;
                return(n-1);
            }else
                return(-1);
        }
        *ptr=0;
        return(n);
    }

    void str_echo(int fd) {
        ssize_t n;
        char buff[MAXLINE];
        while (true) {
            while ((n = read(fd, buff, MAXLINE)) > 0) {
                std::cout << "buff[" << n << "] = " << buff << std::endl;
                write(fd, buff, n);
            }
            if (n == 0) return;
            if (n < 0) {
                std::cout << "str_errno:" << strerror(errno) << std::endl;
                exit(-1);
            }
        }
    }

    void str_cli(FILE *fp, int fd) {
        char sendline[MAXLINE], recvline[MAXLINE];
        while (fgets(sendline, MAXLINE, fp) != NULL) {
            std::cout << "cliread:" << sendline << std::endl;
            write(fd, sendline, strlen(sendline));
            sleep(1);
            if (readline(fd, recvline, MAXLINE) == 0) {
                std::cout << "str_cli:" << strerror(errno) << std::endl;
                exit(-1);
            }
            fputs(recvline, stdout);
        }
    }

    TCP_base::~TCP_base() {
        close(scfd());
    }

    Server::Server(): TCP_base() {
        bzero(&address, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(SERHOST);
        address.sin_port = htons(SERPORT);
        bind(scfd(), (sockaddr*)&address, sizeof(address));
    }

    void Server::run() {
        listen(scfd(), LISTENQ);
        while(true) {
            socklen_t clilen;
            sockaddr_in cliaddr;
            auto connfd = accept(scfd(), (sockaddr*)&cliaddr, &clilen);
            std::cout << address.sin_port << "\n";
            close(connfd);
        }
    }

    Client::Client(char host[]) : TCP_base() {
        bzero(&address, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_port = htons(SERPORT);
        inet_pton(AF_INET, host, &address.sin_addr);
    }
    void Client::run() {
        connect(scfd(), (sockaddr*)&address, sizeof(address));
    }
}

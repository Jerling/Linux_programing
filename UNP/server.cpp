#include <iostream>
#include "unp.h"

extern int errno;

int main(int argc, char *argv[])
{

    auto skfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(skfd >= 0);
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    addr.sin_port = htons(8088);
    int ret = bind(skfd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        std::cout << strerror(errno) << std::endl;
    }
    assert(ret != -1);

    int backlog = 5;
    ret = listen(skfd, backlog);
    assert(ret != -1);
    std::cout << "Listening..." << "\n";
    char buff[500];

    while (true) {
        struct sockaddr_in cliaddr;
        socklen_t len;
        auto connfd = accept(skfd, (sockaddr *)&cliaddr, &len);
        std::cout << inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)) << " : " <<  ntohs(cliaddr.sin_port) << std::endl;
        write(connfd, buff, strlen(buff));
        close(connfd);
    }
    close( skfd );
    return 0;
}

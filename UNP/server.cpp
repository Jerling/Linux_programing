#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <iostream>

extern int errno;

int main(int argc, char *argv[])
{

    auto skfd = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << skfd << "\n";
    assert(skfd >= 0);
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    addr.sin_port = htons(8088);
    int ret = bind(skfd, (struct sockaddr*)&addr, sizeof(addr));
    std::cout << ret << "\n";
    if (ret < 0) {
        std::cout << strerror(errno) << std::endl;
    }
    assert(ret != -1);
    std::cout << "after bind.." << "\n";

    int backlog = 5;
    while (true) {
        ret = listen(skfd, backlog);
        assert(ret != -1);
    }
    close( skfd );
    return 0;
}

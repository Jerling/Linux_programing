#include "unp.h"

void foo(int fd){
    std::cout << fd << std::endl;
}

int main() {
    auto skfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(skfd >= 0);
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERPORT);
    addr.sin_addr.s_addr = htonl(SERHOST);
    int ret = bind(skfd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
      std::cout << strerror(errno) << std::endl;
    }
    assert(ret != -1);

    ret = listen(skfd, LISTENQ);
    assert(ret != -1);
    std::cout << "Listening..." << "\n";

    while (true) {
      struct sockaddr_in cliaddr;
      socklen_t len;
      auto connfd = accept(skfd, (sockaddr *)&cliaddr, &len);
      pid_t child;
      if ((child = fork()) == 0) {
        close(skfd);
        str_echo(connfd);
        close(connfd);
      }
      close(connfd);
    }
    close(skfd);
    return 0;
}

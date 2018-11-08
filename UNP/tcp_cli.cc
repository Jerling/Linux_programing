#include "unp.h"

void foo(int fd){
    std::cout << fd << std::endl;
}

int main(int args, char* argv[]) {
  if (args != 2){
    std::cout << "输入IP即可" << std::endl;
  }
  auto skfd = socket(AF_INET, SOCK_STREAM, 0);
  assert(skfd >= 0);
  struct sockaddr_in addr;
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(SERPORT);
  inet_pton(AF_INET, argv[1], &addr.sin_addr);

  while (true) {
    auto connfd = connect(skfd,(sockaddr*)&addr, sizeof(addr));
    pid_t child;
    if ((child = fork()) == 0) {
      close(skfd);
      str_cli(stdin, connfd);
      close(connfd);
    }
    close(connfd);
  }
  close(skfd);
  return 0;
}

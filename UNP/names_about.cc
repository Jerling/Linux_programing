#include <iostream>
#include "unp.h"

int main()
{
  auto scfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (scfd < 0) {
    std::cout << strerror(errno) << std::endl;
    exit(-1);
  }
  struct sockaddr_in address;
  addr_init(&address, SERPORT, SERHOST);
  bind(scfd, (struct sockaddr*)&address, sizeof(address));

  sockaddr_in cli;
  socklen_t len;

  // ckname函数用于获取与某个套接字关联的本地协议地址
  // getpeername函数用于获取与某个套接字关联的外地协议地址

  // 获取客户的地址信息
  auto connfd = getpeername(scfd, (sockaddr*)&cli, &len);
  std::cout << ntohl(cli.sin_addr.s_addr) << ":" << ntohs(len) << std::endl;
  close(connfd);

  // 获取本地地址信息
  connfd = getsockname(scfd, (sockaddr*)&address, &len);
  std::cout << ntohl(cli.sin_addr.s_addr) << ":" << ntohs(len) << std::endl;
  close(connfd);

  return 0;
}

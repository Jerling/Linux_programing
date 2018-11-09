#include <iostream>
#include "unp.h"

int main()
{
  std::cout << htonl(INADDR_ANY) << std::endl;
  std::cout << htonl(16) << std::endl; // 主机字节序转为网络字节序

  std::cout << ntohs(3328) << std::endl; // 网络字节序转换为主机字节序

  char buff[MAXLINE];

  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  char ip[] = "127.0.0.1";
  inet_pton(AF_INET, ip, &address.sin_addr);  // IP 转换为网络字节序ip

  inet_ntop(AF_INET, &address.sin_addr, buff, sizeof(buff));
  std::cout << buff << std::endl;             // 网络字节序ip 装换为 IP 
  return 0;
}

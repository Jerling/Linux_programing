#include "unp.h"

int main(int args, char *argv[])
{
    if (args != 2) {
        printf("%s\n", "输入IP即可");
        exit(0);
    }
    int sockfd;
    struct sockaddr_in seraddress;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    addr_init(&seraddress, AF_INET, SERPORT);
    inet_pton(AF_INET, argv[1], &seraddress.sin_addr);

    connect(sockfd, (SA*)&seraddress, sizeof(seraddress));

    printf("%s\n", strerror(errno));
    str_cli(stdin, sockfd);

    return 0;
}

#include "unp.h"

int main(int args, char *argv[])
{
    if (args != 2) {
        printf("输入 IP 即可\n");
    }
    int scfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(SERPORT);
    inet_pton(AF_INET, argv[1], &address.sin_addr);

    connect(scfd, (SA*)&address, sizeof(address));

    char buff[MAXLINE + 1];
    int n;
    while ((n = read(scfd, buff, MAXLINE)) > 0) {
        buff[n] = '\0';
        fputs(buff, stdout);
    }

    return 0;
}

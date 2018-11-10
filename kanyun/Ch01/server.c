#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h> // for bzero strerror
#include <unistd.h> // for fork
#include <wait.h>   // for wait, waitpid

#define SA struct sockaddr
#define MAXLINE 1024

int main( void )
{
    int listenfd, connfd;
    struct  sockaddr_in servaddr;
    char    buff[ MAXLINE ];
    time_t  ticks;
    pid_t   pid;

    listenfd = socket( AF_INET, SOCK_STREAM, 0 );

    bzero( &servaddr, sizeof( servaddr ) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(7777);

    bind(listenfd, ( SA * )&servaddr, sizeof(servaddr));
    listen( listenfd, 5 );

    for( ; ; ){
        connfd = accept( listenfd, ( SA * )NULL, NULL);
        pid = fork();
        if ( 0 == pid ){
            ticks = time( NULL );
            snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
            write( connfd, buff, strlen(buff));
            exit(1);
        }
        if ( waitpid(pid, NULL,0 ) != pid ){
            printf("waitpid error");
            exit(1);
        }
        close( connfd );
    }

    return 0;
}

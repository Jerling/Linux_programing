#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <iostream>

#define MAXLINE 1024
#define LISTENQ 1024
#define SERPORT 8089
#define SERHOST INADDR_ANY
typedef void(*callback)(int);
typedef void(*callback_2)(FILE*, int);

static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

extern int errno;

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
  size_t n, rc;
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

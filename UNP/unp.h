#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

extern int errno;

#define MAXLINE 1024
#define LISTENQ 1024
#define SERPORT 8088
#define SERHOST INADDR_ANY
typedef void(*callback)(int);
typedef void(*callback_2)(FILE*, int);



namespace unp {
    class TCP_base
    {
    public:
        TCP_base():scfd_(socket(AF_INET, SOCK_STREAM, 0)) {
            if(scfd_ == -1) {
                fputs(strerror(errno), stdout);
                exit(-1);
            }
        };
        virtual ~TCP_base();
        virtual void run() = 0;
        int scfd() {return scfd_;};
    protected:
    private:
        int scfd_;
    };
    class Server : public TCP_base
    {
    public:
        Server();
        ~Server();
        virtual void run();

    private:
        struct sockaddr_in address;
    };
    class Client: public TCP_base
    {
    public:
        Client(char []);
        ~Client();
        virtual void run();
    private:
        struct sockaddr_in address;
    };

}

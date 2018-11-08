#include "unp.cc"

using namespace unp;

void foo(int fd){
    std::cout << fd << std::endl;
}

int main() {
    Server server;
    server.run();
}

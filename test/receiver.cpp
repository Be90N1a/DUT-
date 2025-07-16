#include "DHManager.hpp"
#include <iostream>
void p(){
    puts("111");
    getchar();
    puts("111");
}

int main() {
    DHManager bob("bob");
    bob.DHbind("tcp://127.0.0.1:6666");
    bob.DHrecv() ;
p();
    int fd=bob.DHconnect("tcp://127.0.0.1:5555","alice");

    
    
    std::string line;
    while (true) {
        std::cout << "Send: ";
        std::getline(std::cin, line);
        if (line == "exit") break;
        bob.DHsend(fd,line);
        
    }
}

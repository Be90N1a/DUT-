#include "DHManager.hpp"
#include <iostream>

void p(){
    puts("111");
    getchar();
    puts("111");
}

int main() {
    DHManager alice("alice");

    alice.DHbind("tcp://127.0.0.1:5555");
    alice.DHrecv();
// p();

    int fd=alice.DHconnect("tcp://127.0.0.1:6666","bob");
p();

    
    std::string line;
    while (true) {
        std::cout << "Send: ";
        std::getline(std::cin, line);
        if (line == "exit") break;
        alice.DHsend(fd,line);
    }

    return 0;
}

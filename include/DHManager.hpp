#pragma once
#include "message.hpp"
#include "crypto_mac.hpp"
#include "crypto_hash.hpp"
#include "crypto_cert.hpp"
#include "crypto_box.hpp"
#include "diffie_hellman.hpp"
#include "ZmqCommunicator.hpp"
#include <string>
#include <vector>

class DHManager {
public:
   
    DHManager(std::string name);
    ~DHManager();
    void DHbind(const std::string& bindAddress);
    int DHconnect(const std::string& bindAddress,std::string name);
    void DHsend(int fd,const std::string& msg); 
    void DHrecv();

private: 
    crypto::DiffieHellman* dh;
    std::vector<ZmqCommunicator*> zcs;
    // std::string secret;
    std::string key_path;
    std::string pub;

};

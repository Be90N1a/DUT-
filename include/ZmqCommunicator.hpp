#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <zmq.hpp>

class ZmqCommunicator {
public:
    std::string secret;
    std::string crt_path;
    using MessageHandler = std::function<void(const std::string&)>;

    ZmqCommunicator(std::string bindAddress, bool isSer,std::string name); 
    ~ZmqCommunicator();

    void startReceiving(MessageHandler handler);
    void stop();

    void send(const std::string& msg);

private:
    zmq::context_t context_;
    zmq::socket_t socket_;
    std::thread receiverThread_;
    std::atomic<bool> running_;
    bool isServer_;
    
};

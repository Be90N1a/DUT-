#include "ZmqCommunicator.hpp"
#include <iostream>

ZmqCommunicator::ZmqCommunicator(std::string bindAddress, bool isSer,std::string name)
    : context_(1), 
      socket_(context_, isSer ? ZMQ_DEALER : ZMQ_DEALER),
      running_(false),
      isServer_(isSer)
{
    crt_path=""; 
    if (isServer_) {
        std::cout<<bindAddress<<std::endl;
        socket_.bind(bindAddress); 
    } else {
        crt_path="./cert/"+name+".cert";
        socket_.connect(bindAddress);
    }
    // puts("222");

}

ZmqCommunicator::~ZmqCommunicator() {
    stop();
} 

void ZmqCommunicator::startReceiving(MessageHandler handler) {
    running_ = true;
    receiverThread_ = std::thread([this, handler]() {
        while (running_) {
            zmq::message_t request;
            auto result = socket_.recv(request, zmq::recv_flags::none);
            if (!result.has_value()) continue;

            std::string msg(static_cast<char*>(request.data()), request.size());
            handler(msg);

            // if (isServer_) {
            //     zmq::message_t reply(2);
            //     memcpy(reply.data(), "OK", 2);
            //     socket_.send(reply, zmq::send_flags::none);
            // }
        }
    });
}

void ZmqCommunicator::stop() {
    running_ = false;
    if (receiverThread_.joinable()) {
        receiverThread_.join();
    }
}

void ZmqCommunicator::send(const std::string& msg) {
    zmq::message_t message(msg.data(), msg.size());
    socket_.send(message, zmq::send_flags::none);

    // if (!isServer_) {
    //     zmq::message_t reply;
    //     socket_.recv(reply, zmq::recv_flags::none);
    //     std::string rep(static_cast<char*>(reply.data()), reply.size());
    //     std::cout << "[Response] " << rep << std::endl;
    // }
}

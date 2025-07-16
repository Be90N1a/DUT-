#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#include <string>

namespace crypto {

class Message{

public:
    Message();
    Message(std::string sig,std::string tag,std::string context);
    Message(std::string all);
    ~Message();
    std::string getSig();
    std::string getTag();
    std::string getCnt();
    void setSig(std::string sig);
    void setTag(std::string sag);
    void setCnt(std::string cnt);
    std::string merge();    //用于把3个成员合并成一个字符串
    void dispatch(std::string all); //用于把一个字符串拆分成三个成员

private:
    std::string sig;   //长度0x100
    std::string tag;    //长度0x40
    std::string context;    //剩下全是context
};
}

#endif
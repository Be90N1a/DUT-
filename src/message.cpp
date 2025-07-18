#include "message.hpp"
#include <stdexcept>

namespace crypto {

// 默认构造函数
Message::Message() : sig(""), tag(""), context("") {}

// 构造函数，直接赋值三个成员
Message::Message(std::string sig, std::string tag, std::string context)
    : sig(std::move(sig)), tag(std::move(tag)), context(std::move(context)) {}

// 构造函数，从一个完整字符串拆分三个成员
Message::Message(std::string all) {
    dispatch(std::move(all));
}

// 析构函数（此处无特殊资源，空实现）
Message::~Message() = default;

// getter
std::string Message::getSig() {
    return sig;
}

std::string Message::getTag() {
    return tag;
}

std::string Message::getCnt() {
    return context;
}

// setter
void Message::setSig(std::string sig) {
    this->sig = std::move(sig);
}

void Message::setTag(std::string tag) {
    this->tag = std::move(tag);
}

void Message::setCnt(std::string cnt) {
    this->context = std::move(cnt);
}

// 合并三个成员为一个字符串
std::string Message::merge() {
    // sig 长度 0x100 = 256 字符
    // tag 长度 0x40 = 64 字符
    // context 其余全部

    // 为保证格式统一，建议sig和tag不足长度时填充空格或0
    std::string out = sig;
    if (out.size() < 0x100)
        out.append(0x100 - out.size(), ' ');
    else if (out.size() > 0x100)
        out = out.substr(0, 0x100);

    std::string t = tag;
    if (t.size() < 0x40)
        t.append(0x40 - t.size(), '\x00');
    else if (t.size() > 0x40)
        t = t.substr(0, 0x40);

    out += t;
    out += context;

    return out;
}

// 拆分字符串为三个成员
void Message::dispatch(std::string all) {
    if (all.size() < (0x100 + 0x40)) {
        throw std::runtime_error("Message dispatch error: input string too short");
    }

    sig = all.substr(0, 0x100);
    tag = all.substr(0x100, 0x40);
    context = all.substr(0x100 + 0x40);
} 

} // namespace crypto

#ifndef CRYPTO_HASH_HPP
#define CRYPTO_HASH_HPP

#include <string>

namespace crypto {

class Hash {
public:
    static std::string sha256(const std::string& data);
    static std::string sha512(const std::string& data);
};

} // namespace crypto

#endif

#ifndef CRYPTO_MAC_HPP
#define CRYPTO_MAC_HPP

#include <string>

namespace crypto {
class MAC {
public:
    static std::string hmac_sha256(const std::string& key, const std::string& data);
};
}
#endif

#include "crypto_mac.hpp"
#include <openssl/hmac.h>
#include <sstream>
#include <iomanip>

namespace crypto {

std::string MAC::hmac_sha256(const std::string& key, const std::string& data) {
    unsigned char* result;
    unsigned int len = 32;
    unsigned char hmac[32];

    result = HMAC(EVP_sha256(), key.data(), key.size(),
                  (const unsigned char*)data.data(), data.size(),
                  hmac, &len);

    std::ostringstream oss;
    for (unsigned int i = 0; i < len; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hmac[i];
    return oss.str();
}

}

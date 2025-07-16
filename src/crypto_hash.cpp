#include "crypto_hash.hpp"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

namespace crypto {

static std::string bytesToHex(const unsigned char* data, int len) {
    std::ostringstream oss;
    for (int i = 0; i < len; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
    return oss.str();
}

std::string Hash::sha256(const std::string& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)data.data(), data.size(), hash);
    return bytesToHex(hash, SHA256_DIGEST_LENGTH);
}

std::string Hash::sha512(const std::string& data) {
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512((const unsigned char*)data.data(), data.size(), hash);
    return bytesToHex(hash, SHA512_DIGEST_LENGTH);
}

} // namespace crypto

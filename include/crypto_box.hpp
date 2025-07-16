#ifndef CRYPTO_BOX_HPP
#define CRYPTO_BOX_HPP

#include <string>
#include <openssl/evp.h>

namespace crypto {

class CryptoBox {
public:
    static std::string encrypt(const std::string& key_hex, const std::string& plaintext);
    static std::string decrypt(const std::string& key_hex, const std::string& ciphertext);
};

} // namespace crypto

#endif

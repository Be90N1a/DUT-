#ifndef CRYPTO_CERT_HPP
#define CRYPTO_CERT_HPP

#include <string>

namespace crypto {

class Certificate {
public:
    static std::string signWithPrivateKey(const std::string& privkey_file, const std::string& message);
    static bool verifyWithCert(const std::string& cert_file, const std::string& message, const std::string& signature);
};

} // namespace crypto

#endif
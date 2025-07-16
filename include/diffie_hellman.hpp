#ifndef DIFFIE_HELLMAN_HPP
#define DIFFIE_HELLMAN_HPP

#include <string>
#include <openssl/bn.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <stdexcept>

namespace crypto {

class DiffieHellman {
public:
    DiffieHellman(); // 构造并生成 p/g
    ~DiffieHellman();
    static void init(int bit =256);
    static void cleanup();

    void generatePrivateKey();
    void generatePublicKey();

    std::string getPublicKeyHex() const;
    std::string getPrimeHex() const;
    std::string getPrivateKeyHex() const;
    std::string getGeneratorHex() const;

    std::string computeSharedSecretHex(const std::string& otherPublicKeyHex) const;

private:
    static BIGNUM *p_;             // 素数
    static BIGNUM *g_;             // 原根
    static bool initialized_;
    static BN_CTX* ctx_; // 静态上下文，共享即可
    BIGNUM *privateKey_;    // 私钥
    BIGNUM *publicKey_;     // 公钥


    BIGNUM* hexToBN(const std::string& hex) const;
};
 
} // namespace crypto

#endif

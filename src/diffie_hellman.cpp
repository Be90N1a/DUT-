#include "diffie_hellman.hpp"
#include <iostream>

namespace crypto {

BIGNUM* crypto::DiffieHellman::p_ = nullptr;
BIGNUM* crypto::DiffieHellman::g_ = nullptr;
bool crypto::DiffieHellman::initialized_ = false;
BN_CTX* crypto::DiffieHellman::ctx_ = nullptr;

DiffieHellman::DiffieHellman() {
    privateKey_ = BN_new();
    publicKey_ = BN_new();

}

void crypto::DiffieHellman::init(int bits) {
    if (initialized_) return;

    ctx_ = BN_CTX_new();
    p_ = BN_new();
    g_ = BN_new();

    BN_hex2bn(&p_, "e6f03f6f711b0c24ff7afe3605a17ab3a11d3e075483aa211958d903f2b41b4b6a6ea1c19bf3144b28ae2575fabe896b1c72b3775a81b3f341ab1ec1adf34f2b");

    // if (!BN_generate_prime_ex(p_, bits, 1, nullptr, nullptr, nullptr))
    //     throw std::runtime_error("Failed to generate prime");

    BN_set_word(g_, 2);
    initialized_ = true;

}

void DiffieHellman::cleanup() {
    if (p_) { BN_free(p_); p_ = nullptr; }
    if (g_) { BN_free(g_); g_ = nullptr; }
    if (ctx_) { BN_CTX_free(ctx_); ctx_ = nullptr; }
    initialized_ = false;
}

DiffieHellman::~DiffieHellman() {
    BN_free(p_);
    BN_free(g_);
}

void DiffieHellman::generatePrivateKey() {
    // 私钥为 [2, p-2) 范围内安全随机数
    BIGNUM *range = BN_dup(p_);
    BN_sub_word(range, 2);
    BN_rand_range(privateKey_, range);
    BN_add_word(privateKey_, 2); // 保证 ≥2
    BN_free(range);
}

void DiffieHellman::generatePublicKey() {
    BN_mod_exp(publicKey_, g_, privateKey_, p_, ctx_);
}

std::string DiffieHellman::getPublicKeyHex() const {
    char* hex = BN_bn2hex(publicKey_);
    std::string result(hex);
    OPENSSL_free(hex);
    return result;
}

std::string DiffieHellman::getPrivateKeyHex() const {
    char* hex = BN_bn2hex(privateKey_);
    std::string result(hex);
    OPENSSL_free(hex);
    return result;
}

std::string DiffieHellman::getPrimeHex() const {
    char* hex = BN_bn2hex(p_);
    std::string result(hex);
    OPENSSL_free(hex);
    return result;
}

std::string DiffieHellman::getGeneratorHex() const {
    char* hex = BN_bn2hex(g_);
    std::string result(hex);
    OPENSSL_free(hex);
    return result;
}

std::string DiffieHellman::computeSharedSecretHex(const std::string& otherPublicKeyHex) const {
    BIGNUM* otherKey = hexToBN(otherPublicKeyHex);
    BIGNUM* secret = BN_new();
    BN_mod_exp(secret, otherKey, privateKey_, p_, ctx_);

    char* hex = BN_bn2hex(secret);
    std::string result(hex);
    OPENSSL_free(hex);

    BN_free(secret);
    BN_free(otherKey);
    return result;
}

BIGNUM* DiffieHellman::hexToBN(const std::string& hex) const {
    BIGNUM* bn = nullptr;
    BN_hex2bn(&bn, hex.c_str());
    return bn;
}

} // namespace crypto

#include "crypto_box.hpp"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdexcept>
#include <vector>
#include <sstream>

namespace crypto {

static std::vector<unsigned char> hexToBytes(const std::string& hex) {
    std::vector<unsigned char> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        unsigned int byte;
        sscanf(hex.substr(i, 2).c_str(), "%02x", &byte);
        bytes.push_back(static_cast<unsigned char>(byte));
    }
    return bytes;
}

std::string CryptoBox::encrypt(const std::string& key_hex, const std::string& plaintext) {
    auto key = hexToBytes(key_hex);
    std::vector<unsigned char> iv(16);
    RAND_bytes(iv.data(), iv.size());

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::vector<unsigned char> ciphertext(plaintext.size() + 16);

    int len = 0, ciphertext_len = 0;

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data());
    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, reinterpret_cast<const unsigned char*>(plaintext.data()), plaintext.size());
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    std::ostringstream oss;
    for (auto b : iv) oss << std::hex << (b >> 4) << (b & 0xf);
    for (int i = 0; i < ciphertext_len; ++i)
        oss << std::hex << (ciphertext[i] >> 4) << (ciphertext[i] & 0xf);

    return oss.str();  // IV + 密文
}

std::string CryptoBox::decrypt(const std::string& key_hex, const std::string& hextext) {
    auto key = hexToBytes(key_hex);
    auto full = hexToBytes(hextext);
    std::vector<unsigned char> iv(full.begin(), full.begin() + 16);
    std::vector<unsigned char> ciphertext(full.begin() + 16, full.end());

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::vector<unsigned char> plaintext(ciphertext.size());

    int len = 0, plaintext_len = 0;

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data());
    EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size());
    plaintext_len = len;
    EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len);
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return std::string(plaintext.begin(), plaintext.begin() + plaintext_len);
}

} // namespace crypto

#include "crypto_cert.hpp"
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <vector>
#include <iostream>

namespace crypto {

std::string Certificate::signWithPrivateKey(const std::string& privkey_file, const std::string& message) {
    FILE* fp = fopen(privkey_file.c_str(), "r");
    EVP_PKEY* pkey = PEM_read_PrivateKey(fp, nullptr, nullptr, nullptr);
    fclose(fp);

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestSignInit(ctx, nullptr, EVP_sha256(), nullptr, pkey);
    EVP_DigestSignUpdate(ctx, message.data(), message.size());

    size_t len;
    EVP_DigestSignFinal(ctx, nullptr, &len);
    std::vector<unsigned char> sig(len); 
    EVP_DigestSignFinal(ctx, sig.data(), &len);

    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    // std::cout << "签名长度（字节）: " << sig.size() << std::endl;
    return std::string(sig.begin(), sig.end());
}

bool Certificate::verifyWithCert(const std::string& cert_file, const std::string& message, const std::string& signature) {
    FILE* fp = fopen(cert_file.c_str(), "r");
    X509* cert = PEM_read_X509(fp, nullptr, nullptr, nullptr);
    fclose(fp);

    EVP_PKEY* pubkey = X509_get_pubkey(cert);
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();

    EVP_DigestVerifyInit(ctx, nullptr, EVP_sha256(), nullptr, pubkey);
    EVP_DigestVerifyUpdate(ctx, message.data(), message.size());
    bool valid = EVP_DigestVerifyFinal(ctx, (const unsigned char*)signature.data(), signature.size()) == 1;

    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pubkey);
    X509_free(cert);
    return valid;
}

} // namespace crypto
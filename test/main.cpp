#include "diffie_hellman.hpp"
#include "crypto_box.hpp"
#include "crypto_hash.hpp"
#include "crypto_mac.hpp"
#include "crypto_cert.hpp"
#include <iostream>
#include <fstream>

int main() {
    crypto::DiffieHellman::init(512);
    crypto::DiffieHellman alice, bob;
    alice.generatePrivateKey(); alice.generatePublicKey();
    bob.generatePrivateKey(); bob.generatePublicKey();

    std::string a_pub = alice.getPublicKeyHex();
    std::string b_pub = bob.getPublicKeyHex();

    // ✅ Alice 对自己的公钥签名
    std::string a_sig = crypto::Certificate::signWithPrivateKey("alice.key", a_pub);

    // ✅ Bob 验证 Alice 的签名（使用 alice.crt）
    if (!crypto::Certificate::verifyWithCert("alice.crt", a_pub, a_sig)) {
        std::cerr << "❌ Alice 的公钥签名无效（MITM？）" << std::endl;
        return -1;
    }
    // puts("111");
    std::string a_secret = alice.computeSharedSecretHex(b_pub);
    std::string b_secret = bob.computeSharedSecretHex(a_pub);

    if (a_secret != b_secret) {
        std::cerr << "❌ DH 密钥不一致！" << std::endl;
        return -1;
    }

    std::string aes_key = crypto::Hash::sha256(a_secret);
    std::string hmac_key = crypto::Hash::sha256(a_secret + "hmac");

    std::string msg = "Hello, this is signed and encrypted!";
    std::string ciphertext = crypto::CryptoBox::encrypt(aes_key, msg);
    std::string tag = crypto::MAC::hmac_sha256(hmac_key, ciphertext);
    // std::cout << "tag长度（字节）: " << tag.size() << std::endl;
    if (crypto::MAC::hmac_sha256(hmac_key, ciphertext) != tag) {
        std::cerr << "❌ 消息被篡改" << std::endl;
    } else {
        std::string plain = crypto::CryptoBox::decrypt(aes_key, ciphertext);
        std::cout << "✅ 解密成功: " << plain << std::endl;
    }

    crypto::DiffieHellman::cleanup();
    return 0;
}

// ===== 证书生成示例命令 =====
// openssl genrsa -out alice.key 2048
// openssl req -new -x509 -key alice.key -out alice.crt -days 365

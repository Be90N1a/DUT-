#include "DHManager.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>  // C++17
#include <string>
DHManager::DHManager(std::string name) {

    namespace fs = std::filesystem;

    // 拼接文件路径
    fs::path key_dir = "key";
    fs::path cert_dir = "cert";

    // 确保目录存在
    if (!fs::exists(key_dir)) {
        fs::create_directory(key_dir);
    }
    if (!fs::exists(cert_dir)) {
        fs::create_directory(cert_dir);
    }

    std::string key_name=(name + ".key");
    std::string cert_name=(name + ".cert");

    fs::path key_file = key_dir / key_name;
    fs::path cert_file = cert_dir / cert_name;

    key_path = key_file.string();
    std::string cert_path = cert_file.string();

    // 生成私钥文件
    if (!fs::exists(key_file)) {
        std::string cmd1 = "openssl genrsa -out " + key_path + " 2048";
        if (system(cmd1.c_str()) != 0) {
            throw std::runtime_error("Failed to generate RSA private key");
        }
        std::cout << "Generated private key: " << key_path << std::endl;
    }

    // 生成自签证书文件（会交互，需要预先准备好配置或自动输入）
    if (!fs::exists(cert_file)) {
        std::string cmd2 = "openssl req -new -x509 -key " + key_path + " -out " + cert_path + " -days 365 -subj \"/CN=" + name + "\"";
        if (system(cmd2.c_str()) != 0) {
            throw std::runtime_error("Failed to generate self-signed certificate");
        }
        std::cout << "Generated certificate: " << cert_path << std::endl;
    }

    crypto::DiffieHellman::init();
    dh=new crypto::DiffieHellman();
    dh->generatePrivateKey(); 
    dh->generatePublicKey();

    pub = dh->getPublicKeyHex();
    

}

DHManager::~DHManager() {
    if (dh) {
        delete dh;
        dh = nullptr;
    }
    for (auto ptr : zcs) {
        delete ptr;  // 释放每个指针指向的对象
    }
    zcs.clear();  // 清空指针容器
}

void DHManager::DHbind(const std::string& bindAddress){

    zcs.resize(1);
    zcs[0]=new ZmqCommunicator(bindAddress,1,"myself"); 
    
}
int DHManager::DHconnect(const std::string& bindAddress,std::string name) {
    // 遍历 zcs 找第一个空指针（nullptr）
    int idx=-1;
    for (int i = 0; i < zcs.size(); ++i) {
        if (zcs[i] == nullptr) {
            zcs[i] = new ZmqCommunicator(bindAddress,0,name);
            idx=i;
        }
    }
    
    if(idx==-1){
        // 如果没找到空位，可以选择扩展数组并添加新元素
        ZmqCommunicator* newZc = new ZmqCommunicator(bindAddress,0,name);
        zcs.push_back(newZc);
        idx=zcs.size() - 1;
    }
    // puts("222");
    // std::cout<< zcs[0]<<std::endl;
    // std::cout<< idx<<std::endl;
    // std::cout<< zcs[idx]<<std::endl;
    std::string sig = crypto::Certificate::signWithPrivateKey(key_path, pub);
    crypto::Message buf(sig,std::string(0x40, '\0'),pub);
    std::string line=buf.merge();
    zcs[idx]->send(line);

    return idx;
}

void DHManager::DHsend(int fd,const std::string& msg){
    if(fd==0)
        return ;
    
    std::string aes_key = crypto::Hash::sha256(zcs[fd]->secret);
    std::string hmac_key = crypto::Hash::sha256(zcs[fd]->secret + "hmac");
    std::string ciphertext = crypto::CryptoBox::encrypt(aes_key, msg);
    std::string tag = crypto::MAC::hmac_sha256(hmac_key, ciphertext);
    std::string sig = crypto::Certificate::signWithPrivateKey(key_path, ciphertext);
    crypto::Message buf(sig,tag,ciphertext);
    std::string line=buf.merge();
    zcs[fd]->send(line);

    return;


}
void DHManager::DHrecv(){
    zcs[0]->startReceiving([this](const std::string& msg) { 
        crypto::Message rcv(msg);
        if(rcv.getTag()==std::string(0x40, '\0')){   //magic
            for (int i = 0; i < zcs.size(); ++i) {
                if (zcs[i] == nullptr) continue;  // 跳过空指针
                if (crypto::Certificate::verifyWithCert(zcs[i]->crt_path, rcv.getCnt(), rcv.getSig())) {
                    zcs[i]->secret = dh->computeSharedSecretHex(rcv.getCnt());
                }
            }
        }else{
            for (int i = 0; i < zcs.size(); ++i) {
                if (zcs[i] == nullptr) continue;  // 跳过空指针
                if (crypto::Certificate::verifyWithCert(zcs[i]->crt_path, rcv.getCnt(), rcv.getSig())) {
                    
                    std::string aes_key = crypto::Hash::sha256(zcs[i]->secret);
                    std::string hmac_key = crypto::Hash::sha256(zcs[i]->secret + "hmac");
                    std::string ciphertext = rcv.getCnt();
                    std::string tag = rcv.getTag();
                    // std::cout << "tag长度（字节）: " << tag.size() << std::endl;
                    if (crypto::MAC::hmac_sha256(hmac_key, ciphertext) != tag) {
                        std::cerr << "❌ 消息被篡改" << std::endl;
                    } else {
                        std::string plain = crypto::CryptoBox::decrypt(aes_key, ciphertext);
                        std::cout << "✅ 解密成功: " << plain << std::endl;
                    }

                }
            }
        }
    });
}
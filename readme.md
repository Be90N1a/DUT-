对diffie_hellman协议的实现，使用zmq实现通信。功能封装在了libdiffie_hellman.so库中。
每一个进程维护一个自己的通信队列，第一个zcs[0]表示本进程用于接收的接口，其他依次为DHconnect建立连接后用于发送的接口
该协议考虑了数字签名，消息签名，消息加密等技术，可以防止中间人攻击和消息的篡改和泄露
发送数据的结构体为message结构体，第一个元素是数字签名，第二个元素是消息签名，第三个元素是加密的消息或者公钥
支持多进程间通信，支持高数据量的并发通信
#include <iostream>
#include <string>
#include "hv/UdpServer.h"
#include "hv/WebSocketClient.h"
#include "module/utils/socket.hpp"
#include "module/tunnel/LibhvWsClientTunnel/LibhvWsClientTunnel.h"
//
//int main() {
//    sockaddr_u sockaddrU;
//    std::string address = "10.0.0.6:7789";
//
//    omg::utils::Socket::parseIPAddress(address, &sockaddrU);
//
//    hv::UdpServer udpServer;
//    udpServer.start();
//    udpServer.sendto(address, reinterpret_cast<sockaddr *>(&sockaddrU));
//
//    return 0;
//}

class MyC {
public:
    MyC(){
        std::cout << "MyC" << std::endl;
    }
    ~MyC(){
        std::cout << "~MyC" << std::endl;
    }
};

int main(){
//    hv::EventLoopPtr eventLoop = std::make_shared<hv::EventLoop>();
//    std::shared_ptr<omg::LibhvWsClientTunnel> tunnel = std::make_shared<omg::LibhvWsClientTunnel>(eventLoop, "ws://124.222.224.186:8800", 1);
//
//    tunnel->onDestroy = [](const omg::TunnelPtr& tunnel){
//        std::cout << "Destroy" << std::endl;
//    };
//
//    tunnel->onReceive = [](const omg::TunnelPtr& tunnel, const omg::Byte* payload, omg::SizeT length){
//        std::string str((char*)payload, length);
//        std::cout << "On Data:" << str << "   len"<< length << std::endl;
//    };
//
//    tunnel->onStateChange = [](const omg::TunnelPtr& tunnel, omg::LibhvWsClientTunnel::State old, omg::LibhvWsClientTunnel::State current) {
//        std::cout << "State change -> prev:" << old << "new:" << current << std::endl;
//    };
//
//    std::string msg = "Hey Kris";
//
//
//
//    getchar();
//    int nwrite = tunnel->send((omg::Byte*)msg.c_str(), 8);
//    std::cout << "nwrite:" <<nwrite << std::endl;
//    getchar();
//    tunnel->destroy();
//    getchar();

//    hv::WebSocketClient wsClient;
//    wsClient.onclose = [](){
//        std::cout << "Destroy" << std::endl;
//    };
//
//    int errCode = wsClient.open("ws://124.222.224.186:8800");
//    std::cout << "ERRCODE:" << errCode << std::endl;
//
//    getchar();
//    wsClient.close();
//    getchar();

    return 0;
}
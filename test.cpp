#include <iostream>
#include <string>
#include "hv/UdpServer.h"
#include "module/tunnel/TunnelFactory.h"
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


int main(){
    hv::EventLoopPtr eventLoop = std::make_shared<hv::EventLoop>();
    omg::TunnelFactory::getInstance().setEventLoopPtr(eventLoop);

    omg::TunnelPtr tunnelPtr = omg::TunnelFactory::getInstance().createTunnel(omg::TransportProtocol::Websocket, "ws://124.222.224.186:8800");

    tunnelPtr->onReceive = [](const omg::TunnelPtr& tunnel, const omg::Byte* payload, size_t length){
        std::cout << "Data Length:" << length << std::endl;
    };

    tunnelPtr->addOnReadyHandler([](const omg::TunnelPtr& tunnel){
        std::cout << "Tunnel Ready:" << tunnel->id() << std::endl;
    });

    tunnelPtr->addOnErrorHandler([](const omg::TunnelPtr& tunnel, void* data){
        std::cout << "Tunnel Ready:" << tunnel->id() << std::endl;
    });

    tunnelPtr->addOnDestroyHandler([](const omg::TunnelPtr& tunnel){
        std::cout << "Tunnel Destroy:" << tunnel->id() << std::endl;
    });
    std::string str("Hey Kris");
    getchar();
    int n = tunnelPtr->send((omg::Byte*)str.c_str(),8);
    std::cout << n << std::endl;
    getchar();
    tunnelPtr->destroy();
    getchar();
    return 0;
}
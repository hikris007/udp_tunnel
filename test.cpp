#include <iostream>
#include <string>
#include <unordered_map>
#include "hv/UdpServer.h"
#include "module/tunnel/TunnelFactory.h"
//
int main22() {
    sockaddr_u sockaddrU;
    std::string address = "10.0.0.6:7789";

    omg::utils::Socket::parseIPAddress(address, &sockaddrU);
    hv::UdpServer udpServer;
    udpServer.createsocket();
    udpServer.start();
    udpServer.sendto(address, reinterpret_cast<sockaddr *>(&sockaddrU));

    return 0;
}


int main(){
    std::unordered_map<omg::TunnelID, omg::TunnelPtr> tunnels;

    hv::EventLoopPtr eventLoop = std::make_shared<hv::EventLoop>();
    omg::TunnelFactory::getInstance().setEventLoopPtr(eventLoop);

    std::string str("Hey Kris");
    const omg::Byte* data = (omg::Byte*)str.c_str();
    size_t length = 8;

    omg::TunnelPtr tunnelPtr = nullptr;
    for(int i=0;i<100;i++){
        tunnelPtr = omg::TunnelFactory::getInstance().createTunnel(omg::TransportProtocol::Websocket, "ws://124.222.224.186:8800");

        tunnelPtr->onReceive = [](const omg::TunnelPtr& tunnel, const omg::Byte* payload, size_t length){
            std::cout << "Data Length:" << length << std::endl;
        };

        tunnelPtr->addOnReadyHandler([&data,length](const omg::TunnelPtr& tunnel){
//        tunnelPtr->addOnReadyHandler([](const omg::TunnelPtr& tunnel){
            std::cout << "Tunnel Ready:" << tunnel->id() << std::endl;
            tunnel->send(data,length);
        });

        tunnelPtr->addOnErrorHandler([](const omg::TunnelPtr& tunnel, void* data){
            std::cout << "Tunnel Ready:" << tunnel->id() << std::endl;
        });

        tunnelPtr->addOnDestroyHandler([](const omg::TunnelPtr& tunnel){
            std::cout << "Tunnel Destroy:" << tunnel->id() << std::endl;
        });

        tunnels.insert({ tunnelPtr->id(), std::move(tunnelPtr) });
    }

    getchar();
    tunnels[1]->destroy();
    getchar();
    return 0;
}
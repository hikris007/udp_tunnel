#include <iostream>
#include <string>
#include <unordered_map>
#include "hv/WebSocketServer.h"
#include "hv/UdpServer.h"
#include "module/utils/socket.hpp"
#include "module/tunnel/TunnelFactory.h"
#include "module/listener/libhvWsListener/LibhvWsListener.h"
#include "module/udpClient/UDPClientFactory.h"
//
//int main22() {
//    sockaddr_u sockaddrU;
//    std::string address = "10.0.0.6:7789";
//
//    omg::utils::Socket::parseIPAddress(address, &sockaddrU);
//    hv::UdpServer udpServer;
//    udpServer.createsocket();
//    udpServer.start();
//    udpServer.sendto(address, reinterpret_cast<sockaddr *>(&sockaddrU));
//
//    return 0;
//}


//int main(){
//    std::unordered_map<omg::TunnelID, omg::TunnelPtr> tunnels;
//
//    hv::EventLoopPtr eventLoop = std::make_shared<hv::EventLoop>();
//    omg::TunnelFactory::getInstance().setEventLoopPtr(eventLoop);
//
//    std::string str("Hey Kris");
//    const omg::Byte* data = (omg::Byte*)str.c_str();
//    size_t length = 8;
//
//    omg::TunnelPtr tunnelPtr = nullptr;
//    for(int i=0;i<100;i++){
//        tunnelPtr = omg::TunnelFactory::getInstance().createTunnel(omg::TransportProtocol::Websocket, "ws://124.222.224.186:8800");
//
//        tunnelPtr->onReceive = [](const omg::TunnelPtr& tunnel, const omg::Byte* payload, size_t length){
//            std::cout << "Data Length:" << length << std::endl;
//        };
//
//        tunnelPtr->addOnReadyHandler([&data,length](const omg::TunnelPtr& tunnel){
////        tunnelPtr->addOnReadyHandler([](const omg::TunnelPtr& tunnel){
//            std::cout << "Tunnel Ready:" << tunnel->id() << std::endl;
//            tunnel->send(data,length);
//        });
//
//        tunnelPtr->addOnErrorHandler([](const omg::TunnelPtr& tunnel, void* data){
//            std::cout << "Tunnel Ready:" << tunnel->id() << std::endl;
//        });
//
//        tunnelPtr->addOnDestroyHandler([](const omg::TunnelPtr& tunnel){
//            std::cout << "Tunnel Destroy:" << tunnel->id() << std::endl;
//        });
//
//        tunnels.insert({ tunnelPtr->id(), std::move(tunnelPtr) });
//    }
//
//    getchar();
//    tunnels[1]->destroy();
//    getchar();
//    return 0;
//}

//struct SockAddrHasher {
//    size_t operator()(const sockaddr_u& addr) const {
//        switch (addr.sa.sa_family) {
//            case AF_INET:
//                return CityHash64(reinterpret_cast<const char*>(&addr.sin), sizeof(struct sockaddr_in));
//            case AF_INET6:
//                return CityHash64(reinterpret_cast<const char*>(&addr.sin6), sizeof(struct sockaddr_in6));
//            default:
//                return CityHash64(reinterpret_cast<const char*>(&addr.sa), sizeof(struct sockaddr));
//        }
//    }
//};
//
//struct SockAddrEqual {
//    bool operator()(const sockaddr_u& lhs, const sockaddr_u& rhs) const {
//        if (lhs.sa.sa_family != rhs.sa.sa_family) {
//            return false;
//        }
//
//        switch (lhs.sa.sa_family) {
//            case AF_INET:
//                return lhs.sin.sin_port == rhs.sin.sin_port &&
//                       lhs.sin.sin_addr.s_addr == rhs.sin.sin_addr.s_addr;
//            case AF_INET6:
//                return lhs.sin6.sin6_port == rhs.sin6.sin6_port &&
//                       memcmp(&lhs.sin6.sin6_addr, &rhs.sin6.sin6_addr, sizeof(in6_addr)) == 0;
//            default:
//                return memcmp(&lhs.sa, &rhs.sa, sizeof(struct sockaddr)) == 0;
//        }
//    }
//};
//
//int main(){
//    std::unordered_map<sockaddr_u, int, SockAddrHasher, SockAddrEqual> addr_map;
//
//    sockaddr_u addr1;
//    memset(&addr1, 0, sizeof(sockaddr_u));
//    sockaddr_set_ipport(&addr1, "1.1.1.1", 7777);
//
//    sockaddr_u addr2;
//    memset(&addr2, 0, sizeof(sockaddr_u));
//    sockaddr_set_ipport(&addr1, "1.2.1.1", 7777);
//
//    addr_map.insert({ addr1, 1111 });
//    addr_map.insert({ addr2, 2222 });
//
//    std::cout << addr_map.find(addr2)->second << std::endl;
//    std::cout << addr_map.find(addr1)->second << std::endl;
//
//    std::cout << omg::utils::Socket::GenerateSockAddrHash(addr1) << std::endl;
//    std::cout << omg::utils::Socket::GenerateSockAddrHash(addr2) << std::endl;
//    std::cout << omg::utils::Socket::GenerateSockAddrHash(addr1) << std::endl;
//    std::cout << omg::utils::Socket::GenerateSockAddrHash(addr2) << std::endl;
//
//    return 0;
//}

//class MyContext {
//public:
//    int id = -1;
//};
//
//typedef std::shared_ptr<MyContext> MyContextPtr;
//
//int main(){
//    int n = 0;
//    hv::WebSocketService wsService;
//    wsService.onopen = [&n](const WebSocketChannelPtr& channel, const HttpRequestPtr& request){
//        MyContextPtr context = std::make_shared<MyContext>();
//        context->id = n++;
//        channel->setContextPtr(context);
//        std::cout << "Open." << context->id << std::endl;
//    };
//    wsService.onclose = [](const WebSocketChannelPtr& channel){
//        MyContextPtr context = channel->getContextPtr<MyContext>();
//        std::cout << "Closed." << context->id << std::endl;
//    };
//    wsService.onmessage = [](const WebSocketChannelPtr& channel, const std::string& data){
//        MyContextPtr context = channel->getContextPtr<MyContext>();
//        std::cout << "onMessage:" << context->id << data << std::endl;
//    };
//
//    HttpService http;
//    http.GET("/ping", [](const HttpContextPtr& ctx) {
//        return ctx->send("pong");
//    });
//
//    hv::WebSocketServer webSocketServer;
//    webSocketServer.registerWebSocketService(&wsService);
//    webSocketServer.registerHttpService(&http);
//    webSocketServer.port = 8899;
//    webSocketServer.start();
//    webSocketServer.stop();
//
//    while (getchar() != '\n');
//    return 0;
//}

//int main(){
//    hv::EventLoopPtr eventLoop = std::make_shared<hv::EventLoop>();
//    std::shared_ptr<omg::LibhvWsListener> listener = std::make_shared<omg::LibhvWsListener>(eventLoop);
//
//    listener->onAccept = [](const omg::TunnelPtr& tunnel){
//        std::cout << "New Tunnel:" << tunnel->id() << std::endl;
//
//        tunnel->onReceive = [](const omg::TunnelPtr& tunnel, const omg::Byte* payload, size_t len){
//            std::cout << "New data length:" << len << std::endl;
//        };
//
//        tunnel->addOnDestroyHandler([](const omg::TunnelPtr& tunnel){
//            std::cout << "close" << std::endl;
//        });
//
//        std::string data = "Hi I'm Server.";
//        tunnel->send((const omg::Byte*)data.c_str(), data.size());
//
////        tunnel->destroy();
//    };
//
//    std::string listen = "0.0.0.0:8896";
//    listener->start(listen);
//    getchar();
//    listener->stop();
//    getchar();
//    return 0;
//}

//int main(){
//    hv::EventLoopPtr eventloop = std::make_shared<hv::EventLoop>();
//    eventloop->runInLoop(nullptr);
//    std::cout << "HEHE" << std::endl;
//    std::shared_ptr<hv::UdpClient> udpclient;
//    omg::UDPClientFactory::createLibhvUDPClient(eventloop, "10.0.0.1:10000", udpclient);
//    udpclient->onMessage = [](const hv::SocketChannelPtr& channel, hv::Buffer* data){
//        std::cout << "DATA:" << data->size() << std::endl;
//    };
//    udpclient->start();
//
//    std::string d = "Hey Kris";
//    while(getchar() != 'c'){
//        udpclient->sendto(d);
//    }
//    return 0;
//}

void test(hidle_t*){

}

int main(){
    hv::EventLoopPtr eventloop = std::make_shared<hv::EventLoop>();

    eventloop->setInterval(1000, [&eventloop](hv::TimerID timerId){
        std::cout << "HH" << std::endl;
        eventloop->stop();
        std::cout << "stop" << std::endl;
    });

    eventloop->run();
    hidle_add()

    getchar();
    eventloop->setInterval(1000, [&eventloop](hv::TimerID timerId){
        std::cout << "HH" << std::endl;
        eventloop->stop();
        std::cout << "stop" << std::endl;
    });
    eventloop->run();


    return 0;
}
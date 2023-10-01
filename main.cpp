#include <iostream>
#include <unordered_set>
#include "hv/EventLoop.h"
#include "hv/UdpServer.h"

int main() {
    hv::EventLoopPtr eventLoopPtr = std::make_shared<hv::EventLoop>();

    eventLoopPtr->setInterval(1000, [](hv::TimerID timerId){
        std::cout << "TimerID:" << timerId << std::endl;
    });

    hv::UdpServer udpServer(eventLoopPtr);
    udpServer.createsocket(7789);
    udpServer.onMessage = [](const hv::SocketChannelPtr& channel, hv::Buffer* buffer){
        std::cout << "Address: " << channel->peeraddr() << " Data: " << (char*)buffer->data() << std::endl;
    };
    udpServer.start();

    eventLoopPtr->run();
    std::cout << "END" << std::endl;
    return 0;
}
//
// Created by Kris Allen on 2023/10/28.
//

#include "UDPClientFactory.h"

int omg::UDPClientFactory::createUDPClient() {
    return 0;
}

int omg::UDPClientFactory::createLibhvUDPClient(hv::EventLoopPtr eventLoop, std::string remoteAddress, std::shared_ptr<hv::UdpClient> &udpClientResult) {
    int errCode = 0;

    std::shared_ptr<hv::UdpClient> udpClient = std::make_shared<hv::UdpClient>(eventLoop);

    std::string ip;
    int port = 0;
    errCode = utils::Socket::SplitIPAddress(remoteAddress, ip, port);
    if(errCode != 0) return -1;

    int fd = udpClient->createsocket(port, ip.c_str());
    if(fd < 0)return -1;

    udpClientResult = udpClient;
    return 0;
}
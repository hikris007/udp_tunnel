#include <iostream>
#include <string>
#include "hv/UdpServer.h"
#include "module/utils/socket.hpp"

int main() {
    sockaddr_u sockaddrU;
    std::string address = "10.0.0.6:7789";

    omg::utils::Socket::parseIPAddress(address, &sockaddrU);

    hv::UdpServer udpServer;
    udpServer.start();
    udpServer.sendto(address, reinterpret_cast<sockaddr *>(&sockaddrU));

    return 0;
}
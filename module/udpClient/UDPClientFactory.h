//
// Created by Kris on 2023/10/5.
//

#ifndef UDP_TUNNEL_UDPCLIENTFACTORY_H
#define UDP_TUNNEL_UDPCLIENTFACTORY_H
#include <string>
#include "hv/UdpClient.h"
#include "../utils/socket.hpp"

namespace omg {
    class UDPClientFactory {
    public:
        static int createUDPClient();
        static int createLibhvUDPClient(hv::EventLoopPtr eventLoop,std::string remoteAddress, std::shared_ptr<hv::UdpClient>& udpClientResult);
    };
}




#endif //UDP_TUNNEL_UDPCLIENTFACTORY_H

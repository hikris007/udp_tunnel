//
// Created by Kris on 2023/10/11.
//

#ifndef UDP_TUNNEL_SERVERPAIRCONTEXT_H
#define UDP_TUNNEL_SERVERPAIRCONTEXT_H

#include "hv/UdpClient.h"
#include "../tunnel/Tunnel.h"

namespace omg {
    class ServerPairContext {
    public:
        std::shared_ptr<hv::UdpClient> _udpClient = nullptr;
        std::weak_ptr<Tunnel> _tunnel;
    };

    typedef std::shared_ptr<ServerPairContext> ServerPairContextPtr;
}


#endif //UDP_TUNNEL_SERVERPAIRCONTEXT_H

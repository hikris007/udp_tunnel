//
// Created by Kris Allen on 2023/9/18.
//

#ifndef UDP_TUNNEL_TUNNELFACTORY_H
#define UDP_TUNNEL_TUNNELFACTORY_H

#include "Tunnel.h"
#include "../../header/typedef.h"

#include "LibhvWsClientTunnel/LibhvWsClientTunnel.h"

namespace omg {

    class TunnelFactory {
    public:
        static Tunnel* createTunnel(TransportProtocol transportProtocol, const std::string& endpoint);

        static hv::EventLoopPtr getEventLoopPtr();
        static void setEventLoopPtr(hv::EventLoopPtr eventLoop);
        static void deleteEventLoopPtr();

    private:
        static Tunnel* createWsTunnel(const std::string& endpoint);

    private:
        static hv::EventLoopPtr _eventLoop = nullptr;
    };

}


#endif //UDP_TUNNEL_TUNNELFACTORY_H

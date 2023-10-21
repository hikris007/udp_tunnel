//
// Created by Kris Allen on 2023/10/20.
//

#ifndef UDP_TUNNEL_LIBHVWSLISTENER_H
#define UDP_TUNNEL_LIBHVWSLISTENER_H

#include <memory>
#include "hv/WebSocketServer.h"
#include "hv/EventLoop.h"
#include "../../utils/socket.hpp"
#include "../../tunnel/LibhvWsTunnel/Server.h"
#include "../Listener.h"

/*
 * Tunnel ----------- shared_ptr ----------- WebsocketChannel
 * WebsocketChannel ------ weak_ptr ------> Tunnel
 */

namespace omg {
    class LibhvWsServerTunnelContext {
    public:
        TunnelPtr tunnel;
    };

    class LibhvWsListener : public Listener {
    public:
        LibhvWsListener(hv::EventLoopPtr eventLoop);
        int start(std::string listenAddress) override;
        int stop() override;

    private:
        hv::EventLoopPtr _eventLoop;
        hv::HttpService _httpService{};
        hv::WebSocketService _webSocketService{};
        std::shared_ptr<hv::WebSocketServer> _webSocketServer;

        std::string _listenAddress;
    };
}


#endif //UDP_TUNNEL_LIBHVWSLISTENER_H

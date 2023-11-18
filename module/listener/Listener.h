//
// Created by Kris Allen on 2023/10/20.
//

#ifndef UDP_TUNNEL_LISTENER_H
#define UDP_TUNNEL_LISTENER_H

#include <functional>

#include "../tunnel/Tunnel.h"

namespace omg {
    class Listener {
    public:
        enum State {
            INITIAL,
            RUNNING,
            STOPPED,
            ERR
        };

        virtual int start(std::string listenAddress) = 0;
        virtual int stop() = 0;

        std::function<void(TunnelPtr tunnel)> onAccept = nullptr;
        std::function<void(void* data)> onError = nullptr;
    };

    typedef std::shared_ptr<Listener> ListenerPtr;
}

#endif //UDP_TUNNEL_LISTENER_H

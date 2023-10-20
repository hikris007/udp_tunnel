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
            RUNNING,
            STOPPED,
            ERROR
        };

        virtual int start() = 0;
        virtual int stop() = 0;

        std::function<void(TunnelPtr* tunnel)> onAccept = nullptr;
        std::function<void(void* data)> onError = nullptr;
    };
}

#endif //UDP_TUNNEL_LISTENER_H

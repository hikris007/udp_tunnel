//
// Created by Kris Allen on 2023/10/19.
//

#ifndef UDP_TUNNEL_SERVERLISTENER_H
#define UDP_TUNNEL_SERVERLISTENER_H

#include <functional>

namespace omg {
    class ServerListener {
    public:

        int start();
        int stop();

        std::function<void()> onAccept = nullptr;
    };
}

#endif //UDP_TUNNEL_SERVERLISTENER_H

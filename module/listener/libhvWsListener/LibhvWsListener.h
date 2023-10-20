//
// Created by Kris Allen on 2023/10/20.
//

#ifndef UDP_TUNNEL_LIBHVWSLISTENER_H
#define UDP_TUNNEL_LIBHVWSLISTENER_H

#include "../Listener.h"

namespace omg {
    class LibhvWsListener : public Listener {
    public:
        int start() override;
        int stop() override;
    };
}


#endif //UDP_TUNNEL_LIBHVWSLISTENER_H

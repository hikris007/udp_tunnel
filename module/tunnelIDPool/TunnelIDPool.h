//
// Created by Kris Allen on 2023/10/28.
//

#ifndef UDP_TUNNEL_TUNNELIDPOOL_H
#define UDP_TUNNEL_TUNNELIDPOOL_H

#include <queue>
#include "../tunnel/Tunnel.h"

namespace omg {
    class TunnelIDPool {
    public:
        explicit TunnelIDPool(size_t poolSize = 100);
        TunnelID getTunnelID();
        void putTunnelID(TunnelID tunnelID);
        int fillPool();
        size_t poolCount();
        size_t poolSize();

    private:
        std::queue<TunnelID> _tunnelIDPool;
        std::mutex _poolLocker;

        TunnelID _lastTunnelID = INVALID_TUNNEL_ID + 1;
        size_t _poolSize = 100;
    };
}

#endif //UDP_TUNNEL_TUNNELIDPOOL_H

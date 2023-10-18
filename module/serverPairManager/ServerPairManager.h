//
// Created by Kris Allen on 2023/10/1.
//

#ifndef UDP_TUNNEL_SERVERPAIRMANAGER_H
#define UDP_TUNNEL_SERVERPAIRMANAGER_H

#include "hv/EventLoop.h"

#include "../tunnel/Tunnel.h"
#include "../udpClient/UDPClientFactory.h"
#include "../pair/Pair.h"
#include "../../header/AppContext.h"
#include "../context/ServerPairContext.h"

namespace omg {
    class ServerPairManager {
    public:
        ServerPairManager(AppContext* appContext);

        void onTunnelOpen(TunnelPtr tunnelPtr);
        size_t onSend(TunnelPtr tunnelPtr, const Byte* payload, size_t length);

    private:
        Int createPair(PairID pairID, PairPtr& pairPtr);

    protected:
    private:
        // 包装
        std::function<size_t(const PairPtr pair,const Byte* payload, size_t len)> handleSend = nullptr;
        std::function<void(const PairPtr&, const Byte* payload, size_t length)> onReceive = nullptr;

        AppContext* _appContext = nullptr;
        std::unordered_map<TunnelID, TunnelPtr> _tunnels; // 传输层的列表
    };
}

#endif //UDP_TUNNEL_SERVERPAIRMANAGER_H
//
// Created by Kris Allen on 2023/10/1.
//

#ifndef UDP_TUNNEL_SERVERPAIRMANAGER_H
#define UDP_TUNNEL_SERVERPAIRMANAGER_H
#include <unordered_map>

#include "hv/EventLoop.h"

#include "../../header/AppContext.h"
#include "../tunnel/Tunnel.h"
#include "../logger/Logger.h"
#include "../udpClient/UDPClientFactory.h"
#include "../context/ServerPairContext.h"
#include "../context/ServerTunnelContext.h"

namespace omg {
    class ServerPairManager {
    public:
        ServerPairManager(AppContext* appContext, hv::EventLoopPtr eventLoop);

        void onTunnelOpen(TunnelPtr tunnelPtr);
        size_t onSend(TunnelPtr tunnelPtr, const Byte* payload, size_t length);

    private:
        int createPair(TunnelPtr tunnel, PairID pairID, PairPtr& pairPtr);

    protected:
    private:
        // 包装
        std::function<size_t(const PairPtr& pair,const Byte* payload, size_t length)> pairSendHandler = nullptr;
        std::function<void(const PairPtr&, const Byte* payload, size_t length)> pairReceiveHandler = nullptr;
        std::function<void(const PairPtr& pair)> pairCloseHandler = nullptr;

        AppContext* _appContext = nullptr;
        hv::EventLoopPtr _eventLoop = nullptr;
        std::unordered_map<TunnelID, TunnelPtr> _tunnels; // 传输层的列表
        std::mutex _tunnelsMutex;
    };
}

#endif //UDP_TUNNEL_SERVERPAIRMANAGER_H

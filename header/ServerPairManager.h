//
// Created by Kris Allen on 2023/10/1.
//

#ifndef UDP_TUNNEL_SERVERPAIRMANAGER_H
#define UDP_TUNNEL_SERVERPAIRMANAGER_H

#include "hv/EventLoop.h"

#include "Tunnel.h"
#include "Context.h"
#include "Pair.h"

class ServerPairManager {
public:
    explicit ServerPairManager(hv::EventLoopPtr eventLoop);

    void onTunnelOpen(TunnelPtr tunnelPtr);
    SizeT onSend(TunnelPtr tunnelPtr, const Byte* payload, SizeT length);

private:
    Int createPair(PairID pairID, PairPtr& pairPtr);

protected:
private:
    // 包装
    std::function<SizeT(const std::shared_ptr<Pair> pair,const Byte* payload, SizeT len)> handleSend = nullptr;

    hv::EventLoopPtr _eventLoop = nullptr;
    std::unordered_map<TunnelID, TunnelPtr> _tunnels; // 传输层的列表
};


#endif //UDP_TUNNEL_SERVERPAIRMANAGER_H

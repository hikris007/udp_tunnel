//
// Created by Kris Allen on 2023/10/1.
//

#ifndef UDP_TUNNEL_SERVERPAIRMANAGER_H
#define UDP_TUNNEL_SERVERPAIRMANAGER_H

#include "Tunnel.h"
#include "Context.h"
#include "Pair.h"

class ServerPairManager {
public:
    ServerPairManager();

    void onTunnelOpen(TunnelPtr tunnelPtr);
    SizeT onSend(TunnelPtr tunnelPtr, const Byte* payload, SizeT length);

private:
    Int createPair(PairID pairID, PairPtr& pairPtr);

protected:
private:
    // 包装
    std::function<void(const std::shared_ptr<Tunnel> tunnel, PayloadPtr payload)> handleSend = nullptr;

    std::unordered_map<TunnelID, TunnelPtr> _tunnels; // 传输层的列表
};


#endif //UDP_TUNNEL_SERVERPAIRMANAGER_H

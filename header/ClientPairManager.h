//
// Created by Kris on 2023/9/11.
//

#ifndef UDP_TUNNEL_CLIENTPAIRMANAGER_H
#define UDP_TUNNEL_CLIENTPAIRMANAGER_H

#include <string>
#include <unordered_map>
#include <queue>

#include "Pair.h"
#include "Tunnel.h"
#include "TunnelFactory.h"
#include "./Config.h"
#include "Context.h"

/**
 * Send: Client -> TunnelClient -> TunnelServer -> Server
 * Recv: Client <- TunnelClient <- TunnelServer <- Server
 */

class ClientPairManager : public std::enable_shared_from_this<ClientPairManager>{
public:
    // 从隧道出来的整个 Pair
    void onRecv(const Byte* payload, SizeT len);

    // Pair 调用这个函数发送数据
    SizeT onSend(PairID pairID, const Byte* payload, SizeT len);

    // 创建一个隧道
    Int createTunnel();

    // 创建一个 Pair
    Int createPair();
protected:

private:
    ClientConfig* clientConfig = nullptr; // 配置项
    std::mutex _locker; // 锁
    std::unordered_map<TunnelID, TunnelPtr> tunnels; // 传输层的列表
    std::unordered_map<TunnelID ,int> tunnelPairCounter; // Key 是传输层的ID 值是空闲数量
    std::queue<TunnelID> availableTunnelIDs; // 存放可用的（有空闲位置的）底层传输层ID
};


#endif //UDP_TUNNEL_CLIENTPAIRMANAGER_H

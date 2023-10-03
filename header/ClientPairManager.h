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
#include "Logger.h"

/**
 * Send: Client -> TunnelClient -> TunnelServer -> Server
 * Recv: Client <- TunnelClient <- TunnelServer <- Server
 */

class ClientPairManager : public std::enable_shared_from_this<ClientPairManager>{
public:
    explicit ClientPairManager(ClientConfig* clientConfig);

    // 从隧道出来的整个 Pair
    void onReceive(TunnelPtr tunnelPtr, const Byte* payload, SizeT len);

    // Pair 调用这个函数发送数据
    SizeT onSend(PairPtr pairPtr, const Byte* payload, SizeT len);

    // Pair 关闭
    void onPairClose(PairPtr pair);

    // 创建一个隧道
    Int createTunnel();

    // 创建一个 Pair
    Int createPair(PairPtr& outputPair);
protected:

private:
    // 因为不支持引用类的成员 所以要做一层转换
    std::function<SizeT(const PairPtr pair,const Byte* payload, SizeT len)> sendHandler = nullptr;
    std::function<void(const PairPtr pair)> onPairCloseHandler = nullptr;

    ClientConfig* _clientConfig = nullptr; // 配置项
    std::mutex _locker; // 锁
    std::unordered_map<TunnelID, TunnelPtr> _tunnels; // 传输层的列表
    std::unordered_map<TunnelID ,int> _tunnelPairCounter; // Key 是传输层的ID 值是空闲数量
    std::queue<TunnelID> _availableTunnelIDs; // 存放可用的（有空闲位置的）底层传输层ID
};


#endif //UDP_TUNNEL_CLIENTPAIRMANAGER_H

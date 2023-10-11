//
// Created by Kris on 2023/9/11.
//

#ifndef UDP_TUNNEL_CLIENTPAIRMANAGER_H
#define UDP_TUNNEL_CLIENTPAIRMANAGER_H

#include <string>
#include <unordered_map>
#include <queue>

#include "../pair/Pair.h"
#include "../tunnel/Tunnel.h"
#include "../tunnel/TunnelFactory.h"
#include "../../header/AppContext.h"
#include "../logger/Logger.h"

/**
 * Send: Client -> TunnelClient -> TunnelServer -> Server
 * Recv: Client <- TunnelClient <- TunnelServer <- Server
 */

namespace omg {
    class ClientPairManager : public std::enable_shared_from_this<ClientPairManager>{
    public:
        explicit ClientPairManager(ClientConfig* clientConfig);

        // 创建一个隧道
        Int createTunnel();

        /*!
         * 创建一个 Pair
         * @param outputPair 以引用的方式返回结果
         * @return 状态码
         * 0成功
         */
        Int createPair(PairPtr& outputPair);

        // 获取所有隧道
        void foreachTunnels(const std::function<void(TunnelPtr&)>& handler);
    protected:

    private:
        // 因为不支持引用类的成员 所以要做一层转换
        std::function<void(TunnelPtr tunnel, const Byte* payload, SizeT len)> onReceiveWrap = nullptr;
        std::function<SizeT(const PairPtr pair,const Byte* payload, SizeT len)> sendHandler = nullptr;
        std::function<void(const PairPtr pair)> onPairCloseHandler = nullptr;

        ClientConfig* _clientConfig = nullptr; // 配置项
        std::mutex _locker; // 锁
        std::unordered_map<TunnelID, TunnelPtr> _tunnels; // 传输层的列表
        std::unordered_map<TunnelID ,int> _tunnelPairCounter; // Key 是传输层的ID 值是空闲数量
        std::queue<TunnelID> _availableTunnelIDs; // 存放可用的（有空闲位置的）底层传输层ID
    };
}



#endif //UDP_TUNNEL_CLIENTPAIRMANAGER_H

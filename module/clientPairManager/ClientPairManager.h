//
// Created by Kris on 2023/9/11.
//

#ifndef UDP_TUNNEL_CLIENTPAIRMANAGER_H
#define UDP_TUNNEL_CLIENTPAIRMANAGER_H

#include <string>
#include <unordered_map>
#include <list>

#include "../logger/Logger.h"
#include "../pair/Pair.h"
#include "../tunnel/Tunnel.h"
#include "../tunnel/TunnelFactory.h"
#include "../../header/AppContext.h"
#include "../context/ClientTunnelContext.h"
#include "../context/ClientPairContext.h"

/**
 * Send: Client -> TunnelClient -> TunnelServer -> Server
 * Recv: Client <- TunnelClient <- TunnelServer <- Server
 */

namespace omg {
    class ClientPairManager : public std::enable_shared_from_this<ClientPairManager>{
    public:
        explicit ClientPairManager(ClientConfig* clientConfig);

        /*!
         * 创建一个隧道
         * @return 错误码
         * 0 成功
         */
        Int createTunnel();

        /*!
         * 创建一个 Pair
         * @param outputPair 以引用的方式返回结果
         * @return 状态码
         * 0 成功
         */
        Int createPair(PairPtr& outputPair);

        // 获取所有隧道
        void foreachTunnels(const std::function<void(const TunnelPtr&)>& handler);
    protected:

    private:
        /*!
         * 从隧道接收到的整个数据 包含 Pair 头
         * 需要Manager处理后调用相关的Pair
         */
        std::function<void(const TunnelPtr& tunnel, const Byte* payload, size_t length)> onReceive = nullptr;

        /*！
         * 当Pair关闭时的清理函数
         */
        std::function<void(const PairPtr& pair)> onPairCloseHandler = nullptr;

        /*!
         * 需要告诉 Pair 怎么发送数据
         */
        std::function<size_t(const PairPtr& pair,const Byte* payload, size_t length)> pairSendHandler = nullptr;

        /*!
         * 隧道关闭时的清理函数
         */
        std::function<void(const TunnelPtr& tunnel)> onTunnelDestroy = nullptr;

        ClientConfig* _clientConfig = nullptr; // 配置项
        std::mutex _locker; // 锁
        std::unordered_map<TunnelID, TunnelPtr> _tunnels; // 传输层的列表
        std::unordered_map<TunnelID ,int> _tunnelPairCounter; // Key 是传输层的ID 值是空闲数量
        std::list<TunnelID> _availableTunnelIDs; // 存放可用的（有空闲位置的）底层传输层ID
    };
}



#endif //UDP_TUNNEL_CLIENTPAIRMANAGER_H

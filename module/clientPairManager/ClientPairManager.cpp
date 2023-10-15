//
// Created by Kris on 2023/9/11.
//

#include "ClientPairManager.h"


omg::ClientPairManager::ClientPairManager(ClientConfig* clientConfig) {
    this->_clientConfig = clientConfig;

    /*!
     * 当从隧道拿到数据后，应该派发
     */
    this->onReceive = [this](const TunnelPtr& tunnel, const Byte* payload, size_t length){
        // 数据是：Header + Payload
        // 如果长度小于等于 sizeof(Header) 就返回
        // 因为没有数据没有任何意义
        if(payload == nullptr || length < (sizeof(PairID) + 1))
            return;

        // 获取 PairID
        PairID pairID = INVALID_PAIR_ID;
        memcpy(&pairID, payload, sizeof(PairID));

        // 根据 PairID 找到 Pair
        // 如果找不到 Pair 则丢弃
        PairPtr pair = nullptr;
        ClientTunnelContextPtr clientTunnelContext = tunnel->getContextPtr<ClientTunnelContext>();
        clientTunnelContext->getPair(pairID, pair);
        if(pair == nullptr)
            return;

        // 如果没有接收处理函数也丢弃
        if(!pair->onReceive)
            return;

        pair->onReceive(pair, payload+sizeof(PairID), length - sizeof(PairID));
    };

    /*!
     * 告诉Pair怎么发送
     */
    this->pairSendHandler = [this](const PairPtr& pair, const Byte* payload, size_t length){
        // 获取 Pair 上下文
        ClientPairContextPtr clientPairContext = pair->getContextPtr<ClientPairContext>();

        // 从上下文中获取所属的隧道
        // 如果没成功就返回 放弃写入
        TunnelPtr tunnel = clientPairContext->tunnel;
        if(tunnel == nullptr)
            return -1;

        return tunnel->send(payload, length);
    };

    // 当 Pair 关闭怎么处理
    this->onPairCloseHandler = [this](const PairPtr& pair){
        // 获取上下文
        ClientPairContextPtr clientPairContext = pair->getContextPtr<ClientPairContext>();

        // 从上下文中获取所属的隧道 & 上下文
        TunnelPtr tunnel = clientPairContext->tunnel;;
        if(tunnel == nullptr) return -1;
        ClientTunnelContextPtr clientTunnelContext = tunnel->getContextPtr<ClientTunnelContext>();

        // 解除关联 & 释放 PairID
        clientTunnelContext->putSeatNumber(pair->id());
        clientTunnelContext->removePair(pair);
    };

    /*!
     * 当隧道关闭的时候处理函数
     */
    this->onTunnelDestroy = [this](const TunnelPtr& tunnel){
        // 获取上下文
        ClientTunnelContextPtr clientTunnelContext = tunnel->getContextPtr<ClientTunnelContext>();

        // 遍历关闭 Pair
        clientTunnelContext->foreachPairs([](const PairPtr& pair){
            pair->close();
        });

        // 从本地 Tunnels 移除
        std::lock_guard<std::mutex> lockGuard(this->_locker);

        auto iterator = std::find(this->_availableTunnelIDs.begin(), this->_availableTunnelIDs.end(), tunnel->id());
        if(iterator != this->_availableTunnelIDs.end())
            this->_availableTunnelIDs.erase(iterator);

        auto iterator2 = this->_tunnelPairCounter.find(tunnel->id());
        if(iterator2 != this->_tunnelPairCounter.end())
            this->_tunnelPairCounter.erase(iterator2);

        auto iterator3 = this->_tunnels.find(tunnel->id());
        if(iterator3 != this->_tunnels.end())
            this->_tunnels.erase(iterator3);

    };
}

omg::Int omg::ClientPairManager::createTunnel() {
    std::lock_guard<std::mutex> lockGuard(this->_locker);

    // 创建隧道 & 包装智能指针
    Tunnel *tunnel = TunnelFactory::createTunnel(this->_clientConfig->transportProtocol, this->_clientConfig->endpoint);
    TunnelPtr tunnelPtr = TunnelPtr(tunnel);

    // 注册事件
    tunnelPtr->onReceive = this->onReceive;
    tunnelPtr->onDestroy = this->onTunnelDestroy;

    // 配置上下文 & 设置上下文
    ClientTunnelContextPtr clientTunnelContext = std::make_shared<ClientTunnelContext>(
          this->_clientConfig->carryingCapacity
    );
    tunnel->setContextPtr(clientTunnelContext);

    // 放入列表
    TunnelID tunnelID = tunnelPtr->id();
    this->_tunnels.insert({ tunnelID, tunnelPtr });

    // 添加到计数列表 & 添加到可用隧道列表
    this->_tunnelPairCounter.insert({tunnelID, 0});
    this->_availableTunnelIDs.push_back(tunnelID);

    return 0;
}

omg::Int omg::ClientPairManager::createPair(PairPtr& outputPair) {
    // 如果没有可用的隧道就先创建
    if(this->_availableTunnelIDs.empty()){
        this->createTunnel();
    }

    std::lock_guard<std::mutex> lockGuard(this->_locker);

    // 获取第一个空闲隧道 & 上下文
    TunnelID tunnelID = this->_availableTunnelIDs.front();
    auto iterator = this->_tunnels.find(tunnelID);

    TunnelPtr tunnel = iterator->second;
    ClientTunnelContextPtr clientTunnelContext = tunnel->getContextPtr<ClientTunnelContext>();

    // 创建 Pair
    PairID pairID = clientTunnelContext->takeSeatNumber();
    PairPtr pair = std::make_shared<Pair>(pairID);

    // 注册回调
    pair->sendHandler = this->pairSendHandler; // 怎么发送
    pair->addOnCloseHandler(this->onPairCloseHandler); // 关闭的时候做什么

    // 配置 Pair 上下文
    ClientPairContextPtr clientPairContext = std::make_shared<ClientPairContext>();
    clientPairContext->tunnel = tunnel;
    pair->setContextPtr(clientPairContext);

    // 关联隧道
    clientTunnelContext->addPair(pair);

    // 在计数器中查找 & 因为要使用这个底层传输 所以计数+1
    auto targetCounter = this->_tunnelPairCounter.find(tunnelID);
    targetCounter->second++;

    // 如果隧道的空位满了就从可用列表中移除
    if(targetCounter->second >= this->_clientConfig->carryingCapacity){
        this->_availableTunnelIDs.pop_front();
    }

    // 返回结果
    outputPair = std::move(pair);

    return 0;
}

void omg::ClientPairManager::foreachTunnels(const std::function<void(const TunnelPtr&)>& handler) {
    std::lock_guard<std::mutex> lockGuard(this->_locker);

    for(auto & _tunnel : this->_tunnels){
        handler(_tunnel.second);
    }
}
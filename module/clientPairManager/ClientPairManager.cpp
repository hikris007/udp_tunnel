//
// Created by Kris on 2023/9/11.
//

#include "ClientPairManager.h"


omg::ClientPairManager::ClientPairManager(ClientConfig* clientConfig) {
    this->_clientConfig = clientConfig;

    this->onReceive = [this](const TunnelPtr& tunnel, const Byte* payload, SizeT length){
        if(payload == nullptr || length <= (sizeof(PairID) + 1))
            return;

        // 获取 PairID
        PairID pairID = INVALID_PAIR_ID;
        memcpy(&pairID, payload, sizeof(PairID));

        // 根据 PairID 找到 Pair
        PairPtr pair = nullptr;
        ClientTunnelContextPtr clientTunnelContext = tunnel->getContextPtr<ClientTunnelContext>();
        clientTunnelContext->getPair(pairID, pair);
        if(pair == nullptr)
            return;

        if(!pair->onReceive)
            return;

        pair->onReceive(pair, payload+sizeof(PairID), len - sizeof(PairID));
    };

    this->pairSendHandler = [this](const PairPtr& pair, const Byte* payload, SizeT length){
        // 获取 Pair 上下文
        ClientPairContextPtr clientPairContext = pair->getContextPtr<ClientPairContext>();

        // 从上下文中获取所属的隧道 ID && 获取隧道
        TunnelID tunnelID = clientPairContext->_tunnelID;
        TunnelPtr tunnel = nullptr;

        auto iterator = this->_tunnels.find(tunnelID);
        if(iterator == this->_tunnels.end())
            return -1;

        tunnel = iterator->second;

        return tunnel->send(payload, len);
    };

    this->onPairCloseHandler = [this](const PairPtr& pair){
        // 获取上下文
        ClientPairContextPtr clientPairContext = pair->getContextPtr<ClientPairContext>();

        // 从上下文中获取所属的隧道 ID && 获取隧道
        TunnelID tunnelID = clientPairContext->_tunnelID;
        TunnelPtr tunnel = nullptr;

        auto iterator = this->_tunnels.find(tunnelID);
        if(iterator == this->_tunnels.end())
            return;

        tunnel = iterator->second;

        ClientTunnelContextPtr clientTunnelContext = tunnel->getContextPtr<ClientTunnelContext>();
        clientTunnelContext->removePair(pair);
    };

    this->onTunnelDestroy = [](const TunnelPtr& tunnel){

    };
}

omg::Int omg::ClientPairManager::createTunnel() {
    std::lock_guard<std::mutex> lockGuard(this->_locker);

    // 创建隧道 & 包装智能指针
    Tunnel *tunnel = TunnelFactory::createTunnel();
    TunnelPtr tunnelPtr = TunnelPtr(tunnel);

    // TODO:注册事件
    tunnelPtr->onReceive = this->onReceive;

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
    this->_availableTunnelIDs.push(tunnelID);

    return 0;
}

omg::Int omg::ClientPairManager::createPair(PairPtr& outputPair) {
    // 如果没有可用的隧道就先创建
    if(this->_availableTunnelIDs.empty()){
        this->createTunnel();
    }

    std::lock_guard<std::mutex> lockGuard(this->_locker);

    // 获取第一个空闲隧道
    TunnelID tunnelID = this->_availableTunnelIDs.front();
    auto iterator = this->_tunnels.find(tunnelID);

    TunnelPtr tunnel = iterator->second;
    ClientTunnelContextPtr clientTunnelContext = tunnel->getContextPtr<ClientTunnelContext>();

    // 创建 Pair
    PairID pairID = clientTunnelContext->get();
    PairPtr pair = std::make_shared<Pair>(pairID);

    // 注册回调
    pair->handleSend = this->sendHandler;
    pair->addOnCloseHandler(this->onPairCloseHandler);

    // Pair 上下文
    ClientPairContextPtr clientPairContext = std::make_shared<ClientPairContext>();
    clientPairContext->_tunnelID = tunnel->id();
    clientPairContext->_clientPairManagerPtr = shared_from_this();
    pair->setContextPtr(clientPairContext);

    // 关联隧道
    clientTunnelContext->addPair(pair);

    // 在计数器中查找 & 因为要使用这个底层传输 所以计数+1
    auto targetCounter = this->_tunnelPairCounter.find(tunnelID);
    targetCounter->second++;

    // 如果隧道的空位满了就从可用列表中移除
    if(targetCounter->second >= this->_clientConfig->carryingCapacity){
        this->_availableTunnelIDs.pop();
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
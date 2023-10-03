//
// Created by Kris on 2023/9/11.
//

#include "header/ClientPairManager.h"

ClientPairManager::ClientPairManager(ClientConfig* clientConfig) {
    this->_clientConfig = clientConfig;

    this->sendHandler = [this](PairPtr pair,const Byte* payload, SizeT len){
        return this->onSend(pair, payload, len);
    };

    this->onPairCloseHandler = [this](PairPtr pair){
        this->onPairCloseHandler(pair);
    };
}

SizeT ClientPairManager::onSend(PairPtr pair, const Byte *payload, SizeT len) {
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
}

void ClientPairManager::onPairClose(PairPtr pair) {
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
}

Int ClientPairManager::createTunnel() {
    std::lock_guard<std::mutex> lockGuard(this->_locker);

    // 创建隧道 & 包装智能指针
    Tunnel *tunnel = TunnelFactory::createTunnel();
    TunnelPtr tunnelPtr = TunnelPtr(tunnel);

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

Int ClientPairManager::createPair(PairPtr& outputPair) {
    std::lock_guard<std::mutex> lockGuard(this->_locker);

    // 如果没有可用的隧道就先创建
    if(this->_availableTunnelIDs.empty()){
        this->createTunnel();
    }

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
    ClientPairContextPtr clientPairContext = ClientPairContextPtr();
    clientPairContext->_tunnelID = tunnel->id();
    clientPairContext->_clientPairManagerPtr = shared_from_this();
    pair->setContextPtr(clientPairContext);

    // 关联隧道
    clientTunnelContext->addPair(pair);

    // 在计数器中查找 & 因为要使用这个底层传输 所以计数+1
    std::unordered_map<TunnelID ,int>::iterator targetCounter = this->_tunnelPairCounter.find(tunnelID);
    targetCounter->second++;

    // 如果隧道的空位满了就从可用列表中移除
    if(targetCounter->second >= this->_clientConfig->carryingCapacity){
        this->_availableTunnelIDs.pop();
    }

    // 返回结果
    outputPair = std::move(pair);

    return 0;
}
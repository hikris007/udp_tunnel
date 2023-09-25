//
// Created by Kris on 2023/9/11.
//

#include "header/ClientPairManager.h"

Int ClientPairManager::createTunnel() {
    // 创建隧道
    Tunnel *tunnel = TunnelFactory::createTunnel();

    // 配置上下文
    ClientTunnelContextPtr clientTunnelContext = std::make_shared<ClientTunnelContext>();

    // 配置 Pair ID 池
    SizeT begin = INVALID_PAIR_ID+1;
    SizeT end = begin + this->clientConfig->carryingCapacity;
    for(SizeT i = begin; i < end; i++){
        clientTunnelContext->put(i);
    }

    // 设置上下文
    tunnel->setContextPtr(clientTunnelContext);

    // 包装智能指针
    TunnelPtr tunnelPtr(tunnel);

    // 放入列表
    TunnelID tunnelID = tunnelPtr->id();
    this->tunnels.insert({ tunnelID, tunnelPtr });

    // 添加到计数列表
    auto insertResult = this->tunnelPairCounter.insert({tunnelID, 0});

    // 如果有剩余空间就添加到可用的隧道列表
    std::unordered_map<TunnelID, int>::iterator targetCounter = insertResult.first;
    if(targetCounter->second < this->clientConfig->carryingCapacity){
        this->availableTunnelIDs.push(tunnelID);
    }

    return 0;
}

Int ClientPairManager::createPair() {
    std::lock_guard<std::mutex> lockGuard(this->_locker);

    // 如果没有可用的隧道就先创建
    if(this->availableTunnelIDs.empty()){
        this->createTunnel();
    }

    // 获取第一个空闲的 TunnelID
    TunnelID tunnelID = this->availableTunnelIDs.front();
    auto iterator = this->tunnels.find(tunnelID);
    TunnelPtr tunnel = iterator->second;
    ClientTunnelContextPtr clientTunnelContext = tunnel->getContextPtr<ClientTunnelContext>();

    // 创建 Pair
    PairID pairID = clientTunnelContext->get();

    ClientPairContextPtr clientPairContext = std::make_shared<ClientPairContext>();
    clientPairContext->_tunnel = tunnel;
    clientPairContext->_clientPairManagerPtr = shared_from_this();

    PairPtr pair = std::make_shared<Pair>(pairID);

    pair->setContextPtr(clientPairContext);

    // 在计数器中查找 & 因为要使用这个底层传输 所以计数+1
    std::unordered_map<TunnelID ,int>::iterator targetCounter = this->tunnelPairCounter.find(tunnelID);
    targetCounter->second++;

    // 如果隧道的空位满了就从可用列表中移除
    if(targetCounter->second >= this->clientConfig->carryingCapacity){
        this->availableTunnelIDs.pop();
    }

    return 0;
}
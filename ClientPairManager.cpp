//
// Created by Kris on 2023/9/11.
//

#include "header/ClientPairManager.h"

ClientPairManager::ClientPairManager() {
    this->sendHandler = [this](PairPtr pair,const Byte* payload, SizeT len){
        return this->onSend(pair, payload, len);
    };

    this->onPairCloseHandler = [this](PairPtr pair){
        this->onPairCloseHandler(pair);
    };
}

SizeT ClientPairManager::onSend(PairPtr pair, const Byte *payload, SizeT len) {
    // 获取上下文
    ClientPairContextPtr clientPairContext = pair->getContextPtr<ClientPairContext>();

    // 从上下文中获取所属的隧道
    std::weak_ptr<Tunnel> tunnel = clientPairContext->_tunnel;

    // TODO: 写入数据

}

void ClientPairManager::onPairClose() {

}

Int ClientPairManager::createTunnel() {
    std::lock_guard<std::mutex> lockGuard(this->_locker);

    // 创建隧道 & 包装智能指针
    Tunnel *tunnel = TunnelFactory::createTunnel();
    TunnelPtr tunnelPtr = TunnelPtr(tunnel);

    // 配置上下文 & 设置上下文
    ClientTunnelContextPtr clientTunnelContext = std::make_shared<ClientTunnelContext>(
          this->clientConfig->carryingCapacity
    );
    tunnel->setContextPtr(clientTunnelContext);

    // 放入列表
    TunnelID tunnelID = tunnelPtr->id();
    this->tunnels.insert({ tunnelID, tunnelPtr });

    // 添加到计数列表 & 添加到可用隧道列表
    this->tunnelPairCounter.insert({tunnelID, 0});
    this->availableTunnelIDs.push(tunnelID);

    return 0;
}

Int ClientPairManager::createPair() {
    std::lock_guard<std::mutex> lockGuard(this->_locker);

    // 如果没有可用的隧道就先创建
    if(this->availableTunnelIDs.empty()){
        this->createTunnel();
    }

    // 获取第一个空闲隧道
    TunnelID tunnelID = this->availableTunnelIDs.front();
    auto iterator = this->tunnels.find(tunnelID);

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
    clientPairContext->_tunnel = tunnel;
    clientPairContext->_clientPairManagerPtr = shared_from_this();
    pair->setContextPtr(clientPairContext);

    // 关联隧道
    clientTunnelContext->addPair(pair);

    // 在计数器中查找 & 因为要使用这个底层传输 所以计数+1
    std::unordered_map<TunnelID ,int>::iterator targetCounter = this->tunnelPairCounter.find(tunnelID);
    targetCounter->second++;

    // 如果隧道的空位满了就从可用列表中移除
    if(targetCounter->second >= this->clientConfig->carryingCapacity){
        this->availableTunnelIDs.pop();
    }

    return 0;
}
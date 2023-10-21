//
// Created by Kris Allen on 2023/10/1.
//

#include "ServerPairManager.h"

omg::ServerPairManager::ServerPairManager(AppContext* appContext) {
    this->_appContext = appContext;

    this->onReceive = [](const PairPtr& pair, const Byte* payload, size_t length){
        // 获取上下文
        ServerPairContextPtr serverPairContext = pair->getContextPtr<ServerPairContext>();

        std::weak_ptr<Tunnel> tunnelPtr = serverPairContext->_tunnel;
        if(tunnelPtr.expired()) return;

        tunnelPtr.lock()->send(payload, length);
    };

    this->handleSend = [this](const PairPtr pair,const Byte* payload, size_t len){
        // 获取上下文
        ServerPairContextPtr serverPairContext = pair->getContextPtr<ServerPairContext>();

        std::shared_ptr<hv::UdpClient> udpClient = serverPairContext->_udpClient;
        return udpClient->sendto(payload, len);
    };
}

void omg::ServerPairManager::onTunnelOpen(TunnelPtr tunnelPtr) {
    // 注册事件
    tunnelPtr->onReceive = [this](TunnelPtr tunnel, const Byte* payload, size_t len){
        this->onSend(tunnel, payload, len);
    };

    this->_tunnels.insert({ tunnelPtr->id(), tunnelPtr });
}

size_t omg::ServerPairManager::onSend(TunnelPtr tunnel, const Byte *payload, size_t length) {
    if(payload == nullptr || length < (sizeof(PairID) + 1))
        return 0;

    // 获取 PairID && 根据 PairID 获取 Pair
    PairID pairID = INVALID_PAIR_ID;
    memcpy(&pairID, payload, sizeof(PairID));

    omg::ServerTunnelContextPtr serverTunnelContext = tunnel->getContextPtr<omg::ServerTunnelContext>();
    PairPtr pair = serverTunnelContext->getPair(pairID);

    // 如果没有就创建
    if(pair == nullptr){
        this->createPair(pairID, pair);
        serverTunnelContext->addPair(pair);
    }

    return pair->send(payload + sizeof(PairID), length - sizeof(PairID));
}

int omg::ServerPairManager::createPair(PairID pairID, PairPtr& pairPtr) {
    PairPtr pair = std::make_shared<Pair>(pairID);

    // TODO: 注册事件
    pair->sendHandler = this->handleSend;

    // 创建上下文 & 设置上下文
    ServerPairContextPtr serverPairContext = std::make_shared<ServerPairContext>();

    UDPClient* udpClientPtr = UDPClientFactory::createUDPClient();
    UDPClientPtr udpClient = std::shared_ptr<UDPClient>(udpClientPtr);

    // TODO: 注册事件
//    udpClient->onReceive = this->onReceive;

    serverPairContext->_udpClient = udpClient;

    pair->setContextPtr(serverPairContext);

    pairPtr = pair;
    return 0;
}
//
// Created by Kris Allen on 2023/10/1.
//

#include "ServerPairManager.h"

omg::ServerPairManager::ServerPairManager(AppContext* appContext) {
    this->_appContext = appContext;

    this->onReceive = [](const PairPtr& pair, const Byte* payload, SizeT length){
        // 获取上下文
        ServerPairContextPtr serverPairContext = pair->getContextPtr<ServerPairContext>();

        std::weak_ptr<Tunnel> tunnelPtr = serverPairContext->_tunnel;
        if(tunnelPtr.expired())
            return;

        tunnelPtr.lock()->send(payload, length);
    };

    this->handleSend = [this](const PairPtr pair,const Byte* payload, SizeT len){
        // 获取上下文
        ServerPairContextPtr serverPairContext = pair->getContextPtr<ServerPairContext>();

        UDPClientPtr udpClient = serverPairContext->_udpClient;
        return udpClient->send(payload, len);
    };
}

void ServerPairManager::onTunnelOpen(TunnelPtr tunnelPtr) {
    // 注册事件
    tunnelPtr->onReceive = [this](TunnelPtr tunnel, const Byte* payload, SizeT len){
        this->onSend(tunnel, payload, len);
    };

    this->_tunnels.insert({ tunnelPtr->id(), tunnelPtr });
}

SizeT ServerPairManager::onSend(TunnelPtr tunnel, const Byte *payload, SizeT len) {
    if(payload == nullptr || len < (sizeof(PairID) + 1))
        return 0;

    PairID pairID = INVALID_PAIR_ID;
    memcpy(&pairID, payload, sizeof(PairID));

    ServerTunnelContextPtr serverTunnelContext = tunnel->getContextPtr<ServerTunnelContext>();

    PairPtr pair = serverTunnelContext->getPair(pairID);

    if(pair == nullptr){
        this->createPair(pairID, pair);
        serverTunnelContext->addPair(pair);
    }

    return pair->send(payload + sizeof(PairID), len - sizeof(PairID));
}

Int ServerPairManager::createPair(PairID pairID, PairPtr& pairPtr) {
    PairPtr pair = std::make_shared<Pair>(pairID);

    // TODO: 注册事件
    pair->handleSend = this->handleSend;

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
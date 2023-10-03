//
// Created by Kris Allen on 2023/10/1.
//

#include "header/ServerPairManager.h"

ServerPairManager::ServerPairManager(hv::EventLoopPtr eventLoop) {
    this->_eventLoop = std::move(eventLoop);

    this->handleSend = [this](const std::shared_ptr<Pair> pair,const Byte* payload, SizeT len){
        // 获取上下文
        ServerPairContextPtr serverPairContext = pair->getContextPtr<ServerPairContext>();

        std::shared_ptr<hv::UdpClient> udpClient = serverPairContext->udpClient;
        return udpClient->sendto(payload, len);
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

    std::shared_ptr<hv::UdpClient> udpClient = std::make_shared<hv::UdpClient>(this->_eventLoop);

    // TODO: 配置数据
//    udpClient->createsocket();

    serverPairContext->udpClient = udpClient;

    pair->setContextPtr(serverPairContext);

    pairPtr = pair;
    return 0;
}
//
// Created by Kris Allen on 2023/10/1.
//

#include "header/ServerPairManager.h"

ServerPairManager::ServerPairManager() {
    this->handleSend = [this](const std::shared_ptr<Tunnel> tunnel, PayloadPtr payload){
        this->onSend(tunnel, payload->data(), payload->length());
    };
}

void ServerPairManager::onTunnelOpen(TunnelPtr tunnelPtr) {
    // 注册事件
    tunnelPtr->onReceive = this->handleSend;

    this->_tunnels.insert({ tunnelPtr->id(), tunnelPtr });
}

SizeT ServerPairManager::onSend(TunnelPtr tunnelPtr, const Byte *payload, SizeT length) {
    if(payload == nullptr || length <= 0)
        return 0;

    PairID pairID = INVALID_PAIR_ID;
    memcpy(&pairID, payload, sizeof(PairID));

    ServerTunnelContextPtr serverTunnelContext = tunnelPtr->getContextPtr<ServerTunnelContext>();

    PairPtr pairPtr = serverTunnelContext->getPair(pairID);

    if(pairPtr == nullptr){
        this->createPair(pairID, pairPtr);
        serverTunnelContext->addPair(pairPtr);
    }

    return pairPtr->send(payload + sizeof(PairID), length - sizeof(PairID));
}

Int ServerPairManager::createPair(PairID pairID, PairPtr& pairPtr) {
    PairPtr pair = std::make_shared<Pair>(pairID);

    // TODO: 注册事件


    // 创建上下文 & 设置上下文
    ServerPairContextPtr serverPairContext = std::make_shared<ServerPairContext>();

    // TODO: EventLoop
    hv::UdpClient* udpClient = new hv::UdpClient();
    // TODO: 配置数据
//    udpClient->createsocket();

    serverPairContext->udpClientPtr = std::unique_ptr<hv::UdpClient>(udpClient);

    pair->setContextPtr(serverPairContext);

    pairPtr = pair;
    return 0;
}
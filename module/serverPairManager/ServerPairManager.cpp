//
// Created by Kris Allen on 2023/10/1.
//

#include "ServerPairManager.h"

omg::ServerPairManager::ServerPairManager(AppContext* appContext, hv::EventLoopPtr eventLoop)
    : _appContext(appContext),
     _eventLoop(std::move(eventLoop))
{

    this->pairReceiveHandler = [](const PairPtr& pair, const Byte* payload, size_t length){
        // 因为没有数据没有任何意义
        if(payload == nullptr || length < 1)
            return;

        // 获取上下文
        ServerPairContextPtr serverPairContext = pair->getContextPtr<ServerPairContext>();
        std::lock_guard<std::mutex> lockGuard(serverPairContext->_dataMutex);

        memcpy(
                serverPairContext->_data + sizeof(PairID),
                payload,
                length
        );

        // 获取 Tunnel
        TunnelPtr tunnel = serverPairContext->_tunnel;
        tunnel->send(serverPairContext->_data, sizeof(PairID)+length);
    };

    this->pairSendHandler = [this](const PairPtr& pair,const Byte* payload, size_t len){
        // 获取上下文
        ServerPairContextPtr serverPairContext = pair->getContextPtr<ServerPairContext>();

        std::shared_ptr<hv::UdpClient> udpClient = serverPairContext->_udpClient;
        return udpClient->sendto(payload, len);
    };

    this->pairCloseHandler = [this](const PairPtr& pair){
        // 获取 Context
        ServerPairContextPtr serverPairContext = pair->getContextPtr<ServerPairContext>();
        if(serverPairContext == nullptr)return;

        // 获取 Tunnel Context
        if(serverPairContext->_tunnel == nullptr)return;
        ServerTunnelContextPtr serverTunnelContext = serverPairContext->_tunnel->getContextPtr<ServerTunnelContext>();
        if(serverTunnelContext == nullptr)return;

        // 解除和 Tunnel 的关联
        serverTunnelContext->removePair(pair);

        // 关闭 UdpClient
        if(serverPairContext->_udpClient == nullptr)return;
        serverPairContext->_udpClient->stop();
    };
}

void omg::ServerPairManager::onTunnelOpen(TunnelPtr tunnel) {
    // 上下文
    ServerTunnelContextPtr serverTunnelContext = std::make_shared<ServerTunnelContext>();
    tunnel->setContextPtr(serverTunnelContext);

    // 注册事件
    tunnel->onReceive = [this](TunnelPtr tunnel, const Byte* payload, size_t len){
        this->onSend(tunnel, payload, len);
    };

    // 隧道关闭时移除
    tunnel->addOnErrorHandler([this](const TunnelPtr& tunnel, void* data){
        std::lock_guard<std::mutex> lockGuard(this->_tunnelsMutex);

        auto iterator = this->_tunnels.find(tunnel->id());
        if(iterator == this->_tunnels.end())return;

        this->_tunnels.erase(iterator);
    });

    // 隧道关闭时移除
    tunnel->addOnDestroyHandler([this](const TunnelPtr& tunnel){
        std::lock_guard<std::mutex> lockGuard(this->_tunnelsMutex);

        auto iterator = this->_tunnels.find(tunnel->id());
        if(iterator == this->_tunnels.end())return;

        this->_tunnels.erase(iterator);
    });

    std::lock_guard<std::mutex> lockGuard(this->_tunnelsMutex);
    this->_tunnels.insert({ tunnel->id(), tunnel });

    LOGGER_INFO("New tunnel:{}", tunnel->id());
}

size_t omg::ServerPairManager::onSend(TunnelPtr tunnel, const Byte *payload, size_t length) {
    if(payload == nullptr || length < (sizeof(PairID) + 1))
        return 0;

    // 获取 PairID && 根据 PairID 获取 Pair
    PairID pairID = INVALID_PAIR_ID;
    memcpy(&pairID, payload, sizeof(PairID));

    omg::ServerTunnelContextPtr serverTunnelContext = tunnel->getContextPtr<omg::ServerTunnelContext>();
    if(serverTunnelContext == nullptr)return -1;

    PairPtr pair = serverTunnelContext->getPair(pairID);

    // 如果没有就创建
    if(pair == nullptr){
        this->createPair(tunnel, pairID, pair);
        serverTunnelContext->addPair(pair);

        ServerPairContextPtr serverPairContext = pair->getContextPtr<ServerPairContext>();
        if(serverPairContext == nullptr || serverPairContext->_udpClient == nullptr)return -1;
        LOGGER_INFO("New pair:{} <----> {}", pair->id(), serverPairContext->_udpClient->channel->peeraddr());
    }

    return pair->send(payload + sizeof(PairID), length - sizeof(PairID));
}

int omg::ServerPairManager::createPair(TunnelPtr tunnel, PairID pairID, PairPtr& pairPtr) {
    int errCode = 0;

    PairPtr pair = std::make_shared<Pair>(pairID);

    // 注册 Pair 回调
    pair->sendHandler = this->pairSendHandler;
    pair->onReceive = this->pairReceiveHandler;
    pair->addOnCloseHandler(this->pairCloseHandler);

    // 创建 && 配置 UDP Client
    std::shared_ptr<hv::UdpClient> udpClient;
    errCode = UDPClientFactory::createLibhvUDPClient(this->_eventLoop, this->_appContext->serverConfig->endpoint, udpClient);
    if(errCode != 0){
        LOGGER_WARN("Failed to create udp client: {}", errCode);
        return -1;
    }

    udpClient->onMessage = [pair](const hv::SocketChannelPtr& channel, hv::Buffer* data){
        if(pair->onReceive == nullptr)return;

        pair->onReceive(pair, static_cast<const Byte*>(data->data()), data->size());
    };

    // 创建上下文 & 设置上下文
    ServerPairContextPtr serverPairContext = std::make_shared<ServerPairContext>();

    serverPairContext->_udpClient = udpClient;
    serverPairContext->_tunnel = std::move(tunnel);
    memcpy(serverPairContext->_data, &pairID, sizeof(PairID));

    pair->setContextPtr(serverPairContext);

    udpClient->start();

    pairPtr = pair;
    return 0;
}
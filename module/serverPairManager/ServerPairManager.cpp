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
        if(serverPairContext == nullptr)return;

        serverPairContext->payload.copyFrom(payload, sizeof(PairID), length);

        // 记录最后收到数据的时间戳(ms)
        serverPairContext->lastDataReceivedTime = utils::Time::GetCurrentTs();

        // 获取 Tunnel
        const TunnelPtr& tunnel = serverPairContext->_tunnel;
        tunnel->send(serverPairContext->payload.data(), sizeof(PairID)+length);
    };

    this->pairSendHandler = [this](const PairPtr& pair,const Byte* payload, size_t length){
        // 获取上下文
        ServerPairContextPtr serverPairContext = pair->getContextPtr<ServerPairContext>();
        if(serverPairContext == nullptr)return -1;

        // 记录最后发送的时间戳
        serverPairContext->lastDataSentTime = utils::Time::GetCurrentTs();

        std::shared_ptr<hv::UdpClient> udpClient = serverPairContext->_udpClient;
        return udpClient->sendto(payload, length);
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
    // 设置上下文
    ServerTunnelContextPtr serverTunnelContext = std::make_shared<ServerTunnelContext>();
    tunnel->setContextPtr(serverTunnelContext);

    // 注册事件
    tunnel->onReceive = [this](const TunnelPtr& tunnel, const Byte* payload, size_t length){
        this->onSend(tunnel, payload, length);
    };

    // 隧道关闭时移除
    tunnel->addOnErrorHandler([this](const TunnelPtr& tunnel, void* data){
        // 遍历关闭 Pair
        ServerTunnelContextPtr _serverTunnelContext = tunnel->getContextPtr<ServerTunnelContext>();
        if(_serverTunnelContext != nullptr){

            std::vector<PairPtr> pendingToDelete;

            _serverTunnelContext->foreachPairs([&pendingToDelete](const PairPtr& pair){
                pendingToDelete.push_back(pair);
            });

            for(const auto &pair : pendingToDelete){
                LOGGER_INFO("Pair (id: {}) is ready to close, because owner tunnel (id: {}) is close", pair->id(), tunnel->id());
                pair->close();
            }
        }

        std::lock_guard<std::mutex> lockGuard(this->_tunnelsMutex);

        auto iterator = this->_tunnels.find(tunnel->id());
        if(iterator == this->_tunnels.end())return;

        this->_tunnels.erase(iterator);
    });

    // 隧道关闭时移除
    tunnel->addOnDestroyHandler([this](const TunnelPtr& tunnel){
        // 遍历关闭 Pair
        ServerTunnelContextPtr _serverTunnelContext = tunnel->getContextPtr<ServerTunnelContext>();
        if(_serverTunnelContext != nullptr){

            std::vector<PairPtr> pendingToDelete;

            _serverTunnelContext->foreachPairs([&pendingToDelete](const PairPtr& pair){
                pendingToDelete.push_back(pair);
            });

            for(const auto &pair : pendingToDelete){
                LOGGER_INFO("Pair (id: {}) is ready to close, because owner tunnel (id: {}) is close", pair->id(), tunnel->id());
                pair->close();
            }
        }

        std::lock_guard<std::mutex> lockGuard(this->_tunnelsMutex);

        auto iterator = this->_tunnels.find(tunnel->id());
        if(iterator == this->_tunnels.end())return;

        this->_tunnels.erase(iterator);
    });

    std::lock_guard<std::mutex> lockGuard(this->_tunnelsMutex);
    this->_tunnels.insert({ tunnel->id(), tunnel });

    LOGGER_INFO("New tunnel:{}", tunnel->id());
}

size_t omg::ServerPairManager::onSend(const TunnelPtr& tunnel, const Byte *payload, size_t length) {
    int errCode = 0;

    if(payload == nullptr || length < (sizeof(PairID) + 1))
        return 0;

    // 解析 PairID
    PairID pairID = INVALID_PAIR_ID;
    memcpy(&pairID, payload, sizeof(PairID));
    if(pairID == INVALID_PAIR_ID){
        LOGGER_DEBUG("Failed to parse pair id from payload");
        return -1;
    }

    // 尝试从上下文中获取 Pair
    omg::ServerTunnelContextPtr serverTunnelContext = tunnel->getContextPtr<omg::ServerTunnelContext>();
    if(serverTunnelContext == nullptr){
        LOGGER_WARN("Failed to get context from tunnel (id: {}), context is null", tunnel->id());
        return -1;
    }

    PairPtr pair = serverTunnelContext->getPair(pairID);
    bool isNewPair = false;
    // 如果没有就创建
    if(pair == nullptr){
        errCode = this->createPair(tunnel, pairID, pair);

        if(errCode != 0){
            pair->close();
            return -1;
        }

        isNewPair = true;
        LOGGER_INFO("Pair (tunnelID:{}, id: {}) has created", tunnel->id(), pair->id());
    }

    ServerPairContextPtr serverPairContext = pair->getContextPtr<ServerPairContext>();

    if(serverPairContext == nullptr){
        return -1;
    }

    if(isNewPair){
        LOGGER_INFO("Pair (tunnelID:{}, id: {}) map to {}",tunnel->id(), pair->id(), serverPairContext->_udpClient->channel->peeraddr());
    }

    return pair->send(payload + sizeof(PairID), length - sizeof(PairID));
}

int omg::ServerPairManager::createPair(TunnelPtr tunnel, PairID pairID, PairPtr& pairPtr) {
    int errCode = 0;

    ServerTunnelContextPtr serverTunnelContext = tunnel->getContextPtr<ServerTunnelContext>();
    if(serverTunnelContext == nullptr)return -1;

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
    serverPairContext->payload.copyFrom(&pairID, 0, sizeof(PairID));

    pair->setContextPtr(serverPairContext);
    this->_eventLoop->runInLoop(std::bind(&hv::UdpClient::startRecv, udpClient));

    serverTunnelContext->addPair(pair);
    pairPtr = pair;

    return 0;
}

void omg::ServerPairManager::foreachTunnels(const std::function<void(const TunnelPtr &)> &handler) {
    std::lock_guard<std::mutex> lockGuard(this->_tunnelsMutex);

    for(auto & _tunnel : this->_tunnels){
        handler(_tunnel.second);
    }
}
//
// Created by Kris on 2023/9/11.
//

#include "ClientPairManager.h"


omg::ClientPairManager::ClientPairManager(ClientConfig* clientConfig)
    : _pairCounter(clientConfig->carryingCapacity)
{
    this->_clientConfig = clientConfig;

    /*!
     * 当从隧道拿到数据后，应该派发
     */
    this->onReceive = [this](const TunnelPtr& tunnel, const Byte* payload, size_t length){
        // 数据是：Header + Payload
        // 如果长度小于等于 sizeof(Header) 就返回
        // 因为没有数据没有任何意义
        if(payload == nullptr || length < (sizeof(PairID) + 1))
            return;

        // 获取 PairID
        PairID pairID = INVALID_PAIR_ID;
        memcpy(&pairID, payload, sizeof(PairID));

        // 根据 PairID 找到 Pair
        // 如果找不到 Pair 则丢弃
        ClientTunnelContextPtr clientTunnelContext = tunnel->getContextPtr<ClientTunnelContext>();
        if(clientTunnelContext == nullptr) return;

        PairPtr pair = nullptr;
        clientTunnelContext->getPair(pairID, pair);
        if(pair == nullptr) return;

        // 如果没有接收处理函数也丢弃
        if(!pair->onReceive) return;

        pair->onReceive(pair, payload+sizeof(PairID), length - sizeof(PairID));
    };

    /*!
     * 告诉Pair怎么发送
     */
    this->pairSendHandler = [this](const PairPtr& pair, const Byte* payload, size_t length) -> omg::size_t {
        // 获取 Pair 上下文
        ClientPairContextPtr clientPairContext = pair->getContextPtr<ClientPairContext>();
        if(clientPairContext == nullptr) return -1;

        // 记录最后发送时间
        clientPairContext->lastDataSentTime = utils::Time::GetCurrentTs();

        // 从上下文中获取所属的隧道
        // 如果没成功就返回 放弃写入
        TunnelPtr tunnel = clientPairContext->tunnel;
        if(tunnel == nullptr) return -1;

        // 状态是成功连接才写入数据
        if(tunnel->state().current != Tunnel::CONNECTED) return -1;

        return tunnel->send(payload, length);
    };

    // 当 Pair 关闭怎么处理
    this->onPairCloseHandler = [this](const PairPtr& pair) -> void{
        ClientPairContextPtr clientPairContext = pair->getContextPtr<ClientPairContext>();
        if(clientPairContext == nullptr)return;

        if(clientPairContext->tunnel == nullptr)return;

        this->_pairCounter.pairCounterOperate(clientPairContext->tunnel->id(), false);
    };

    /*!
     * 当隧道关闭的时候处理函数
     */
    this->onTunnelDestroy = [this](const TunnelPtr& tunnel){
        // 获取上下文
        ClientTunnelContextPtr clientTunnelContext = tunnel->getContextPtr<ClientTunnelContext>();
        if(clientTunnelContext == nullptr){
            LOGGER_INFO("Failed to get tunnel's context, tunnel id: {}", tunnel->id());
            return;
        }

        // 遍历关闭 Pair
        std::vector<PairPtr> pendingToDelete;

        clientTunnelContext->foreachPairs([&pendingToDelete](const PairPtr& pair){
            pendingToDelete.push_back(pair);
        });

        for(const auto &pair : pendingToDelete){
            LOGGER_INFO("Pair (id: {}) is ready to close, because owner tunnel (id: {}) is close", pair->id(), tunnel->id());
            pair->close();
        }

        // 从本地 Tunnels 移除
        this->removeTunnel(tunnel->id());
    };

    this->onTunnelError = [this](const TunnelPtr& tunnel, void* data){
        // 获取上下文
        ClientTunnelContextPtr clientTunnelContext = tunnel->getContextPtr<ClientTunnelContext>();
        if(clientTunnelContext == nullptr){
            LOGGER_INFO("Failed to get tunnel's context, tunnel id: {}", tunnel->id());
            return;
        }

        // 遍历关闭 Pair
        std::vector<PairPtr> pendingToDelete;

        clientTunnelContext->foreachPairs([&pendingToDelete](const PairPtr& pair){
            pendingToDelete.push_back(pair);
        });

        for(const auto &pair : pendingToDelete){
            LOGGER_INFO("Pair (id :{}) is ready to close, because owner tunnel (id: {}) an error occurred", pair->id(), tunnel->id());
            pair->close();
        }

        // 从本地 Tunnels 移除
        this->removeTunnel(tunnel->id());
    };
}

void omg::ClientPairManager::prepareTunnels() {
    bool noAvailableTunnel = this->_pairCounter.availableTunnelIDList().empty();

    if(noAvailableTunnel){
        int errCode = this->createTunnel();
        if(errCode != 0) {
            LOGGER_WARN("Failed to create tunnel, error code: {}", errCode);
            return;
        }
    }
}

int omg::ClientPairManager::cleanUpUselessTunnels() {
    size_t cts = utils::Time::GetCurrentTs();
    size_t sinceLastClean = cts - this->_lastCleanTime;

    // 1s
    if(this->_lastCleanTime > 0 && sinceLastClean < 1000)
        return 0;

    bool hasAvailableTunnel = false;
    std::vector<TunnelPtr> uselessTunnels;
    size_t beginTs = utils::Time::GetCurrentTs();

    // 获取所有没有 Pair 的隧道
    this->foreachTunnels([&hasAvailableTunnel, &uselessTunnels](const TunnelPtr& tunnel) -> bool {
        ClientTunnelContextPtr clientTunnelContext = tunnel->getContextPtr<ClientTunnelContext>();
        if(clientTunnelContext == nullptr)
            return true;

        // 如果还没有可用隧道 && 有隧道有空位
        if(!hasAvailableTunnel && clientTunnelContext->vacancy() > 0){
            hasAvailableTunnel = true;
            return true;
        }

        if(clientTunnelContext->vacancy() >= clientTunnelContext->capacity())
            uselessTunnels.push_back(tunnel);

        return true;
    });

    // 存在没用的隧道就关闭
    if(!uselessTunnels.empty()){
        for(const auto &tunnel : uselessTunnels){
            LOGGER_INFO("Tunnel (id: {}) is ready to close, because has no pairs", tunnel->id());
            tunnel->destroy();
        }
    }

    this->_lastCleanTime = utils::Time::GetCurrentTs();
    size_t endTs = utils::Time::GetCurrentTs();

    if(!uselessTunnels.empty())
        LOGGER_INFO(
                "Clean up useless tunnels, spend {}ms, success to clean {}/{} tunnels, since last clean is been {} ms",
                endTs - beginTs,
                uselessTunnels.size(),
                this->_tunnels.size(),
                sinceLastClean
        );

    return uselessTunnels.size();
}

int omg::ClientPairManager::createTunnel() {
    std::lock_guard<std::mutex> lockGuard(this->_locker);

    // 创建隧道
    TunnelPtr tunnel = TunnelFactory::getInstance().createTunnel(
            this->_clientConfig->transportProtocol,
            this->_clientConfig->endpoint
    );
    if(tunnel == nullptr) return -1;

    // 注册事件
    tunnel->onReceive = this->onReceive;
    tunnel->addOnDestroyHandler(this->onTunnelDestroy);
    tunnel->addOnErrorHandler(this->onTunnelError);

    // 配置上下文 & 设置上下文
    ClientTunnelContextPtr clientTunnelContext = std::make_shared<ClientTunnelContext>(
          tunnel,
          this->_clientConfig->carryingCapacity
    );
    tunnel->setContextPtr(clientTunnelContext);

    // 放入列表
    TunnelID tunnelID = tunnel->id();
    this->_tunnels.insert({ tunnelID, tunnel });

    // 添加到计数列表
    this->_pairCounter.addTunnel(tunnelID);

    LOGGER_INFO("Tunnel (id: {}) has created", tunnel->id());
    return 0;
}

int omg::ClientPairManager::removeTunnel(omg::TunnelID tunnelID) {
    std::lock_guard<std::mutex> lockGuard(this->_locker);

    // 从本地 Tunnels 移除
    auto iterator = this->_tunnels.find(tunnelID);
    if(iterator != this->_tunnels.end())
        this->_tunnels.erase(iterator);

    // 计数列表移除
    this->_pairCounter.removeTunnel(tunnelID);
    return 0;
}

int omg::ClientPairManager::getAvailableTunnel(omg::TunnelPtr &outputTunnel) {
    std::lock_guard<std::mutex> lockGuard(this->_locker);

    // 获取第一个空闲隧道 & 上下文
    TunnelID tunnelID = this->_pairCounter.getAvailableTunnelID();
    if(tunnelID == INVALID_TUNNEL_ID) return -1;

    auto iterator = this->_tunnels.find(tunnelID);
    if(iterator == this->_tunnels.end()) return -1;

    TunnelPtr tunnel = iterator->second;
    if(tunnel == nullptr)return -1;

    outputTunnel = tunnel;
    return 0;
}

int omg::ClientPairManager::createPair(PairPtr& outputPair) {
    this->prepareTunnels();

    int errCode = 0;

    // 获取隧道
    TunnelPtr tunnel = nullptr;
    errCode = this->getAvailableTunnel(tunnel);
    if(errCode != 0)return -1;

    // 正常来说隧道一定会有上下文 如果没有直接关了
    ClientTunnelContextPtr clientTunnelContext = tunnel->getContextPtr<ClientTunnelContext>();
    if(clientTunnelContext == nullptr){
        tunnel->destroy();
        return -1;
    }

    // 创建 Pair
    PairID pairID = clientTunnelContext->takeSeatNumber();
    if(pairID == INVALID_PAIR_ID) return -1;

    PairPtr pair = std::make_shared<Pair>(pairID);

    // 注册回调
    pair->sendHandler = this->pairSendHandler; // 怎么发送
    pair->addOnCloseHandler(this->onPairCloseHandler); // 关闭的时候做什么

    // 配置 Pair 上下文
    ClientPairContextPtr clientPairContext = std::make_shared<ClientPairContext>();
    clientPairContext->tunnel = tunnel;
    pair->setContextPtr(clientPairContext);

    errCode = clientTunnelContext->bindPair(pair);
    if(errCode != 0)return -1;

    // 返回结果
    outputPair = std::move(pair);

    return 0;
}

void omg::ClientPairManager::foreachTunnels(const std::function<bool(const TunnelPtr&)>& handler) {
    std::lock_guard<std::mutex> lockGuard(this->_locker);

    for(auto & _tunnel : this->_tunnels){
        if(!handler(_tunnel.second))
            break;
    }
}
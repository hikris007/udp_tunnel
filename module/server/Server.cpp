//
// Created by Kris Allen on 2023/10/2.
//

#include "Server.h"

omg::Server::Server(AppContext* appContext)
    : _appContext(appContext),
      _eventLoop(std::make_shared<hv::EventLoop>()),
      _serverPairManager(std::make_shared<ServerPairManager>(this->_appContext, this->_eventLoop))
{
    omg::ListenerFactory::getInstance().createListener(
            this->_appContext->transportProtocol,
            this->_appContext->serverConfig->listenDescription,
            this->_listener
    );
}

int omg::Server::shutdown() {
    if(!this->_isRunning) return -1;

    std::lock_guard<std::mutex> lockGuard(this->_shutdownMutex);

    this->_listener->stop();

    // 停止 GC 清理
    if(this->gcTimerID != INVALID_TIMER_ID){
        this->_eventLoop->killTimer(this->gcTimerID);
        LOGGER_WARN("GC is stopped, timer id: {}", this->gcTimerID);

        this->gcTimerID = INVALID_TIMER_ID;
    }

//    if(this->_eventLoop->isRunning())
//        this->_eventLoop->stop();

    this->_isRunning = false;

    LOGGER_WARN("The server is shutdown");
    return 0;
}

int omg::Server::run() {
    if(this->_isRunning) return -1;

    std::lock_guard<std::mutex> lockGuard(this->_runMutex);

    this->_listener->onAccept = [this](TunnelPtr tunnel){
        this->_serverPairManager->onTunnelOpen(std::move(tunnel));
    };

    this->_listener->start(this->_appContext->serverConfig->listenDescription);

    // 垃圾回收
    this->gcTimerID = this->_eventLoop->setInterval(1000 * 10, [this](hv::TimerID timerID){
        this->garbageCollection();
    });

//    if(this->_eventLoop->isStopped())
//        this->_eventLoop->run();

    LOGGER_WARN("GC is running, timer id: {}", this->gcTimerID);

    this->_isRunning = true;
    LOGGER_WARN("The server is running on {}", this->_appContext->serverConfig->listenDescription);

    return 0;
}

void omg::Server::garbageCollection() {
    std::lock_guard<std::mutex> lockGuard(this->_gcMutex);

    LOGGER_DEBUG("GC start running");

    size_t beginTimestamp = utils::Time::GetCurrentTs();
    size_t endTimestamp = 0;
    int tunnelCount = 0;
    int pairCount = 0;

    std::vector<PairPtr> pendingToDelete;

    auto pairHandler = [this, &pairCount, &pendingToDelete](PairPtr& pair){
        pairCount++;

        // 获取 Pair 上下文
        ServerPairContextPtr serverPairContext = pair->getContextPtr<ServerPairContext>();
        if(serverPairContext == nullptr){
            LOGGER_DEBUG("Pair's context is null, pair id: {}", tunnel->id());
            return;
        }

        // 当前时间戳(毫秒)
        size_t cts = omg::utils::Time::GetCurrentTs();

        // 差
        size_t sinceLastWrite = cts - serverPairContext->lastDataSentTime;
        size_t sinceLastReceived = cts - serverPairContext->lastDataReceivedTime;
        LOGGER_DEBUG("Pair last activity time, sinceLastWrite: {}, sinceLastReceived:{}, pair id: {}", sinceLastWrite, sinceLastReceived, pair->id());

        // 超时就关闭
        if(sinceLastWrite > this->_appContext->writeTimeout || sinceLastReceived > this->_appContext->receiveTimeout){
            LOGGER_DEBUG("Clean pair, id: {}",  pair->id());

            pendingToDelete.push_back(pair);
        }
    };

    auto handler = [&pairHandler, &tunnelCount](const TunnelPtr& tunnel){
        tunnelCount++;

        // 获取 Tunnel 上下文
        ServerTunnelContextPtr serverTunnelContext = tunnel->getContextPtr<ServerTunnelContext>();
        if(serverTunnelContext == nullptr){
            LOGGER_DEBUG("Tunnel's context is null, tunnel id: {}", tunnel->id());
            return;
        }

        // 遍历 Pair
        LOGGER_DEBUG("Foreach pairs of tunnel, tunnel id: {}", tunnel->id());
        serverTunnelContext->foreachPairs(pairHandler);
    };

    int cleanPairCount = 0;
    this->_serverPairManager->foreachTunnels(handler);
    for(const auto &pair : pendingToDelete){
        pair->close();
        cleanPairCount++;
    }

    endTimestamp = utils::Time::GetCurrentTs();

    LOGGER_INFO(
            "GC finished，spend {}ms, tunnel count: {}, success to clean {}/{} pairs",
            ( endTimestamp - beginTimestamp ),
            tunnelCount,
            pairCount,
            cleanPairCount
    );
}
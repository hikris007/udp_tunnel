//
// Created by Kris Allen on 2023/9/28.
//

#include "Client.h"

omg::Client::Client(AppContext *config) {
    this->_appContext = config;
    this->_eventLoop = std::make_shared<hv::EventLoop>();
    this->_clientPairManager = std::make_shared<ClientPairManager>(this->_appContext->clientConfig);
    this->_clientForwarder = std::unique_ptr<ClientForwarder>(new ClientForwarder(this->_clientPairManager));
    this->_udpServer = std::unique_ptr<hv::UdpServer>(new hv::UdpServer(this->_eventLoop));

    // 初始化全局的隧道构造器的 EventLoop
    TunnelFactory::getInstance().setEventLoopPtr(this->_eventLoop);
}

void omg::Client::garbageCollection() {
    std::lock_guard<std::mutex> lockGuard(this->_gcMutex);

    LOGGER_DEBUG("GC start running");

    size_t beginTimestamp = utils::Time::GetCurrentTs();
    size_t endTimestamp = 0;
    int tunnelCount = 0;
    int pairCount = 0;
    int cleanPairCount = 0;

    auto pairHandler = [this, &pairCount, &cleanPairCount](PairPtr& pair){
        pairCount++;

        // 获取 Pair 上下文
        ClientPairContextPtr clientPairContext = pair->getContextPtr<ClientPairContext>();
        if(clientPairContext == nullptr){
            LOGGER_DEBUG("Pair's context is null, pair id: {}", tunnel->id());
            return;
        }

        // 当前时间戳(毫秒)
        size_t cts = omg::utils::Time::GetCurrentTs();

        // 差
        size_t sinceLastWrite = cts - clientPairContext->_lastDataSentTime;
        size_t sinceLastReceived = cts - clientPairContext->_lastDataReceivedTime;
        LOGGER_DEBUG("Pair last activity time, sinceLastWrite: {}, sinceLastReceived:{}, pair id: {}", sinceLastWrite, sinceLastReceived, pair->id());

        // 超时就关闭
        if(sinceLastWrite > this->_appContext->writeTimeout || sinceLastReceived > this->_appContext->receiveTimeout){
            LOGGER_DEBUG("Clean pair, id: {}",  pair->id());

            pair->close();
            cleanPairCount++;
        }
    };

    auto handler = [&pairHandler, &tunnelCount](const TunnelPtr& tunnel){
        tunnelCount++;

        // 获取 Tunnel 上下文
        ClientTunnelContextPtr clientTunnelContext = tunnel->getContextPtr<ClientTunnelContext>();
        if(clientTunnelContext == nullptr){
            LOGGER_DEBUG("Tunnel's context is null, tunnel id: {}", tunnel->id());
            return;
        }

        // 遍历 Pair
        LOGGER_DEBUG("Foreach pairs of tunnel, tunnel id: {}", tunnel->id());
        clientTunnelContext->foreachPairs(pairHandler);
    };

    this->_clientPairManager->foreachTunnels(handler);
    endTimestamp = utils::Time::GetCurrentTs();

    LOGGER_INFO(
            "GC finished， spend {}ms, tunnel count: {}, success to clean {}/{} pairs",
            ( endTimestamp - beginTimestamp ),
            tunnelCount,
            pairCount,
            cleanPairCount
    );
}

int omg::Client::init() {
    // ------- 创建套接字 -------

    // 解析地址
    std::string ip;
    int port;

    int errCode = utils::Socket::SplitIPAddress(
            this->_appContext->clientConfig->listenDescription,
            ip,
            port
    );
    if(errCode != utils::Socket::ParseErrorCode::SUCCESS){
        LOGGER_ERROR("Failed to parse listen listen description, error code: {}", errCode);
        return -1;
    }

    int fd = this->_udpServer->createsocket(port, ip.c_str());
    if(fd < 0){
        LOGGER_ERROR("Failed to create socket, fd: {}", fd);
        return -1;
    }

    // ------- 注册事件 -------

    // 当从本地接收到包就写入处理
    this->_udpServer->onMessage = [this](const hv::SocketChannelPtr& channel, hv::Buffer* buffer){
        sockaddr* sourceSocketAddress = hio_peeraddr(channel->io_);
        this->_clientForwarder->onSend(
                sourceSocketAddress,
                static_cast<const Byte*>(buffer->data()),
                buffer->size()
        );
    };

    // 收到响应包就写回去
    this->_clientForwarder->onReceive = [this](const PairPtr& pairPtr, const Byte* payload, size_t length){
        // 获取 Pair 上下文
        ClientPairContextPtr clientPairContext = pairPtr->getContextPtr<ClientPairContext>();

        // 从上下文中获取源地址
        sockaddr_u* sourceAddress = &clientPairContext->_sourceAddressSockAddr;

        // 记录最后收到数据的时间
        clientPairContext->_lastDataReceivedTime = utils::Time::GetCurrentTs();

        // 写回去
        return this->_udpServer->sendto(
                payload,
                length,
                reinterpret_cast<sockaddr *>(sourceAddress)
        );
    };

    return 0;
}

int omg::Client::run() {
    if(this->isRunning)
        return 0;

    std::lock_guard<std::mutex> locker(this->_runMutex);

    // 初始化
    int errorCode = this->init();
    if(errorCode != 0){
        LOGGER_ERROR("Failed to init, error code: {}", errorCode);
        return -1;
    }

    // 垃圾回收
    this->gcTimerID = this->_eventLoop->setInterval(1000 * 10, [this](hv::TimerID timerID){
        this->garbageCollection();
    });
    LOGGER_WARN("GC is running, timer id: {}", this->gcTimerID);

    // 开始
    this->_udpServer->start();

    this->isRunning = true;
    LOGGER_WARN("The client is running on {}.", this->_appContext->clientConfig->listenDescription);

    return 0;
}

int omg::Client::shutdown() {
    if(!this->isRunning)
        return 0;

    std::lock_guard<std::mutex> lockGuard(this->_shutdownMutex);

    // 停止 GC 清理
    if(this->gcTimerID != INVALID_TIMER_ID){
        this->_eventLoop->killTimer(this->gcTimerID);
        LOGGER_WARN("GC is stopped, timer id: {}", this->gcTimerID);

        this->gcTimerID = INVALID_TIMER_ID;
    }

    // 停止接收数据
    this->_udpServer->stop();

    this->isRunning = false;

    LOGGER_WARN("The client is shutdown.");
    return 0;
}
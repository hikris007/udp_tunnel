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
    LOGGER_INFO("garbageCollection start.");
    auto pairHandler = [this](PairPtr& pair){
        // 获取 Pair 上下文
        ClientPairContextPtr clientPairContext = pair->getContextPtr<ClientPairContext>();

        // 当前时间戳(毫秒)
        size_t cts = omg::utils::Time::getCurrentTs();

        // 差
        size_t sinceLastWrite = cts - clientPairContext->_lastDataSentTime;
        size_t sinceLastReceived = cts - clientPairContext->_lastDataReceivedTime;

        // 超时就关闭
        if(sinceLastWrite > this->_appContext->writeTimeout || sinceLastReceived > this->_appContext->receiveTimeout){
            pair->close();
        }
    };

    auto handler = [&pairHandler](const TunnelPtr& tunnel){
        // 获取 Tunnel 上下文
        ClientTunnelContextPtr clientTunnelContext = tunnel->getContextPtr<ClientTunnelContext>();

        // 遍历 Pair
        clientTunnelContext->foreachPairs(pairHandler);
    };
    LOGGER_INFO("garbageCollection end.");

    this->_clientPairManager->foreachTunnels(handler);
}

int omg::Client::init() {
    // ------- 创建套接字 -------

    // 解析地址
    std::string ip;
    int port;

    int errCode = utils::Socket::splitIPAddress(
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
        LOGGER_DEBUG("Receive data length: {} from {}.", buffer->size(), channel->peeraddr());

        this->_clientForwarder->onSend(
                channel->peeraddr(),
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
        this->gcTimerID = INVALID_TIMER_ID;
    }

    // 停止接收数据
    this->_udpServer->stop();

    this->isRunning = false;

    LOGGER_WARN("The client is shutdown.");
    return 0;
}
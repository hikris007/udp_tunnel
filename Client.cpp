//
// Created by Kris Allen on 2023/9/28.
//

#include "header/Client.h"

Client::Client(AppContext *config) {
    this->_appContext = config;
    this->_eventLoop = std::make_shared<hv::EventLoop>();
    this->_clientPairManager = std::make_shared<ClientPairManager>(this->_appContext->clientConfig);
    this->_clientForwarder = std::unique_ptr<ClientForwarder>(new ClientForwarder(this->_clientPairManager));
    this->_udpServer = std::unique_ptr<hv::UdpServer>(new hv::UdpServer(this->_eventLoop));

    this->init();
}

SizeT Client::getCurrentTs() {
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();

    // 转换为自纪元以来的毫秒数
    auto epoch_time = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(epoch_time).count();

    return millis;
}

void Client::garbageCollection() {
    std::lock_guard<std::mutex> lockGuard(this->_locker);

    auto pairHandler = [this](PairPtr& pair){
        // 获取 Pair 上下文
        ClientPairContextPtr clientPairContext = pair->getContextPtr<ClientPairContext>();

        // 当前时间戳(毫秒)
        SizeT cts = this->getCurrentTs();

        // 差
        SizeT sinceLastWrite = cts - clientPairContext->_lastDataSentTime;
        SizeT sinceLastReceived = cts - clientPairContext->_lastDataReceivedTime;

        // 超时就关闭
        if(sinceLastWrite > this->_appContext->writeTimeout || sinceLastReceived > this->_appContext->receiveTimeout){
            pair->close();
        }
    };

    auto handler = [&pairHandler](TunnelPtr& tunnel){
        // 获取 Tunnel 上下文
        ClientTunnelContextPtr clientTunnelContext = tunnel->getContextPtr<ClientTunnelContext>();

        // 遍历 Pair
        clientTunnelContext->foreachPairs(pairHandler);
    };
    this->_clientPairManager->foreachTunnels(handler);
}

Int Client::init() {
    int fd = this->_udpServer->createsocket(8899);
    if(fd < 0){
        Logger::getInstance().getLogger()->error("Failed to create socket, fd: {}", fd);
        return -1;
    }

    // 注册事件

    // 当从本地接收到包就写入处理
    this->_udpServer->onMessage = [this](const hv::SocketChannelPtr& channel, hv::Buffer* buffer){
        Logger::getInstance().getLogger()->debug("Receive data length: {} from {}.", buffer->size(), channel->peeraddr());
        this->_clientForwarder->onSend(
                channel->peeraddr(),
                static_cast<Byte*>(buffer->data()),
                buffer->size()
        );
    };

    // 收到响应包就写回去
    this->_clientForwarder->onReceive = [this](PairPtr pairPtr, Byte* payload, SizeT length){
        // 获取 Pair 上下文
        ClientPairContextPtr clientPairContext = pairPtr->getContextPtr<ClientPairContext>();

        // 从上下文中获取源地址
        sockaddr* sourceAddress = clientPairContext->_sourceAddressSockAddr;

        // 写回去
        return this->_udpServer->sendto(
                payload,
                length,
                sourceAddress
        );
    };

    return 0;
}

Int Client::run() {
    std::lock_guard<std::mutex> locker(this->_locker);

    if(this->isRunning)
        return 0;

    // 垃圾回收
    this->gcTimerID = this->_eventLoop->setInterval(5000, [this](hv::TimerID timerID){
        this->garbageCollection();
    });

    // 开始
    this->_udpServer->start();

    this->isRunning = true;
    Logger::getInstance().getLogger()->warn("The client is running on {}.", this->_appContext->clientConfig->listenDescription);

    return 0;
}

Int Client::shutdown() {
    Logger::getInstance().getLogger()->debug("Client->shutdown()");
    if(!this->isRunning)
        return 0;

    if(this->gcTimerID != INVALID_TIMER_ID){
        this->_eventLoop->killTimer(this->gcTimerID);
    }

    this->_udpServer->stop();

    this->isRunning = false;
    Logger::getInstance().getLogger()->warn("The client is shutdown.");
    return 0;
}
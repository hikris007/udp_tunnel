//
// Created by Kris Allen on 2023/9/28.
//

#include "header/Client.h"

Client::Client(Config *config) {
    this->_config = config;
    this->_eventLoopPtr = std::make_shared<hv::EventLoop>();
    this->_clientForwarderPtr = std::unique_ptr<ClientForwarder>(new ClientForwarder());
    this->_udpServerPtr = std::unique_ptr<hv::UdpServer>(new hv::UdpServer(this->_eventLoopPtr));
}

Int Client::init() {
    int fd = this->_udpServerPtr->createsocket(8899);
    if(fd < 0){
        // TODO: 待测试
        Logger::getInstance().getLogger()->error("Failed to create socket, fd: {}", fd);
        return -1;
    }

    // 注册事件

    // 当从本地接收到包就写入处理
    this->_udpServerPtr->onMessage = [this](const hv::SocketChannelPtr& channel, hv::Buffer* buffer){
        this->_clientForwarderPtr->onSend(
                channel->peeraddr(),
                static_cast<Byte*>(buffer->data()),
                buffer->size()
        );
    };

    // 收到响应包就写回去
    this->_clientForwarderPtr->onReceive = [this](PairPtr pairPtr, Byte* payload, SizeT length){
        // 获取 Pair 上下文
        ClientPairContextPtr clientPairContext = pairPtr->getContextPtr<ClientPairContext>();

        // 从上下文中获取源地址
        sockaddr* sourceAddress = clientPairContext->_sourceAddressSockAddr;

        // 写回去
        return this->_udpServerPtr->sendto(
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
    this->gcTimerID = this->_eventLoopPtr->setInterval(5000, [this](hv::TimerID timerID){
        this->garbageCollection();
    });

    // 开始
    this->_udpServerPtr->start();

    this->isRunning = true;
    return 0;
}

Int Client::shutdown() {
    if(!this->isRunning)
        return 0;

    if(this->gcTimerID != INVALID_TIMER_ID){
        this->_eventLoopPtr->killTimer(this->gcTimerID);
    }

    this->_udpServerPtr->stop();

    this->isRunning = false;

    return 0;
}
//
// Created by Kris Allen on 2023/10/20.
//

#include "LibhvWsListener.h"

omg::LibhvWsListener::LibhvWsListener(hv::EventLoopPtr eventLoop)
    : _eventLoop(std::move(eventLoop)),
      _webSocketServer(std::make_shared<hv::WebSocketServer>())
{
    this->_webSocketService.onopen = [this](const WebSocketChannelPtr& channel, const HttpRequestPtr& request){

        // 如果 TunnelID 池 <=10/1 就重新生成
        if(this->_tunnelIDPool.poolCount() <= (this->_tunnelIDPool.poolSize() / 10))
            this->_tunnelIDPool.fillPool();

        std::shared_ptr<LibhvWsServerTunnelContext> websocketChannelContext = std::make_shared<LibhvWsServerTunnelContext>();

        // 构造隧道
        TunnelID tunnelID = this->_tunnelIDPool.getTunnelID();
        std::shared_ptr<LibhvWsServerTunnel> tunnel = std::make_shared<LibhvWsServerTunnel>(tunnelID, channel);

        // 设置 Channel 上下文
        websocketChannelContext->tunnel = tunnel;
        channel->setContextPtr(websocketChannelContext);

        // 回调
        tunnel->addOnErrorHandler([this](const TunnelPtr& tunnel, void* data){
            this->_tunnelIDPool.putTunnelID(tunnel->id());
        });
        tunnel->addOnDestroyHandler([this](const TunnelPtr& tunnel){
            this->_tunnelIDPool.putTunnelID(tunnel->id());
        });

        // 通知回调
        if(this->onAccept)
            this->onAccept(tunnel);
    };

    this->_webSocketService.onmessage = [this](const WebSocketChannelPtr& channel, const std::string& data){
        // 获取上下文
        std::shared_ptr<LibhvWsServerTunnelContext> websocketChannelContext = channel->getContextPtr<LibhvWsServerTunnelContext>();

        // 如果有所属的 Tunnel && 有设置接收的处理函数就调用
        if(websocketChannelContext->tunnel == nullptr)
            return;

        TunnelPtr tunnel = websocketChannelContext->tunnel;
        if(tunnel->onReceive == nullptr)
            return;

        const Byte* payload = reinterpret_cast<const Byte*>(data.c_str());

        tunnel->onReceive(tunnel, payload, data.size());
    };

    this->_webSocketService.onclose = [this](const WebSocketChannelPtr& channel){
        // 获取上下文
        std::shared_ptr<LibhvWsServerTunnelContext> websocketChannelContext = channel->getContextPtr<LibhvWsServerTunnelContext>();

        // 如果有所属的 Tunnel && 有设置接收的处理函数就调用
        if(websocketChannelContext->tunnel == nullptr)
            return;

        TunnelPtr tunnel = websocketChannelContext->tunnel;
        tunnel->destroy();
    };

    this->_webSocketServer->registerHttpService(&this->_httpService);
    this->_webSocketServer->registerWebSocketService(&this->_webSocketService);
}

int omg::LibhvWsListener::start(std::string listenAddress) {
    if(this->_isRunning) return -1;
    std::lock_guard<std::mutex> lockGuard(this->_runMutex);

    this->_listenAddress = listenAddress;

    std::string ip;
    int port = 0;

    int errorCode = utils::Socket::SplitIPAddress(this->_listenAddress, ip, port);
    if(errorCode != 0) return -1;

    this->_webSocketServer->setHost(ip.c_str());
    this->_webSocketServer->setPort(port);

    this->_webSocketServer->run(false);
    this->_isRunning = true;

    return 0;
}

int omg::LibhvWsListener::stop() {
    this->_webSocketServer->stop();
    this->_isRunning = false;

    return 0;
}
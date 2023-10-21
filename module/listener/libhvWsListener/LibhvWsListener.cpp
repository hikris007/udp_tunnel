//
// Created by Kris Allen on 2023/10/20.
//

#include "LibhvWsListener.h"

omg::LibhvWsListener::LibhvWsListener(hv::EventLoopPtr eventLoop)
    : _eventLoop(std::move(eventLoop)),
      _webSocketServer(std::make_shared<hv::WebSocketServer>())
{
    this->_webSocketService.onopen = [this](const WebSocketChannelPtr& channel, const HttpRequestPtr& request){
        std::shared_ptr<LibhvWsServerTunnelContext> websocketChannelContext = std::make_shared<LibhvWsServerTunnelContext>();

        // 构造隧道
        // TODO: id
        std::shared_ptr<omg::LibhvWsServerTunnel> tunnel = std::make_shared<omg::LibhvWsServerTunnel>(1, channel);

        // 设置 Channel 上下文
        websocketChannelContext->tunnel = tunnel;
        channel->setContextPtr(websocketChannelContext);

        // 通知回调
        if(this->onAccept)
            this->onAccept(tunnel);
    };

    this->_webSocketService.onmessage = [this](const WebSocketChannelPtr& channel, const std::string& data){
        // 获取上下文
        std::shared_ptr<omg::LibhvWsServerTunnelContext> websocketChannelContext = channel->getContextPtr<omg::LibhvWsServerTunnelContext>();

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
        std::shared_ptr<omg::LibhvWsServerTunnelContext> websocketChannelContext = channel->getContextPtr<omg::LibhvWsServerTunnelContext>();

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
    this->_listenAddress = listenAddress;

    std::string ip;
    int port = 0;

    int errorCode = omg::utils::Socket::SplitIPAddress(this->_listenAddress, ip, port);
    if(errorCode != 0) return -1;

    this->_webSocketServer->setHost(ip.c_str());
    this->_webSocketServer->setPort(port);

    this->_webSocketServer->run();
    return 0;
}

int omg::LibhvWsListener::stop() {
    this->_webSocketServer->stop();
    return 0;
}
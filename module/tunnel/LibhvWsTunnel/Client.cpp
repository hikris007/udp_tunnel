//
// Created by Kris Allen on 2023/10/17.
//

#include "Client.h"
#include <utility>

namespace omg {
    using WsClient = hv::WebSocketClient;
    using WsClientUniquePtr = std::unique_ptr<WsClient>;

    LibhvWsClientTunnel::LibhvWsClientTunnel(hv::EventLoopPtr eventLoop, std::string url, omg::TunnelID tunnelID)
        : _state({INITIAL, INITIAL}),
          _tunnelID(tunnelID),
          _url(std::move(url)),
          _webSocketClient(WsClientUniquePtr(new WsClient(std::move(eventLoop))))
    {
        // 成功连接
        this->_webSocketClient->onopen = [this](){
            this->changeState(CONNECTED);

            this->_onReadyCallbacks.trigger(shared_from_this());
        };

        // 处理消息
        this->_webSocketClient->onmessage = [this](const std::string& data){
            // 如果没有处理函数就返回
            if(!this->onReceive) return;

            const Byte* payload = reinterpret_cast<const Byte*>(data.c_str());
            this->onReceive(shared_from_this(), payload, data.size());
        };

        // 关闭回调
        this->_webSocketClient->onclose = [this](){
            if(this->_state.current == CONNECTING){
                // 出错了
                this->changeState(ERROR);

                // 调用回调
                this->_onErrorCallbacks.trigger(shared_from_this(), nullptr);
                return;
            }

            if(this->_state.current == CONNECTED)
                this->changeState(DESTROYING);

            this->changeState(DESTROYED);

            // 触发关闭回调
            this->_onDestroyCallbacks.trigger(shared_from_this());
        };

        // 设置心跳包间隔
        this->_webSocketClient->setPingInterval(1000*5);

        // 设置重连策略
        this->_reconnSetting.min_delay = 1000;
        this->_reconnSetting.max_delay = 1000 * 10;
        this->_reconnSetting.delay_policy = 2;
        this->_reconnSetting.max_retry_cnt = 20;
        this->_webSocketClient->setReconnect(&this->_reconnSetting);

        // 发起连接
        this->_webSocketClient->open(this->_url.c_str());

        // 连接中状态
        this->changeState(CONNECTING);
    }
    int LibhvWsClientTunnel::destroy() {
        this->changeState(DESTROYING);
        this->_webSocketClient->close();
        return 0;
    }

    void LibhvWsClientTunnel::changeState(omg::Tunnel::State state) {
        // 如果当前状态和要切换的状态一样就跳过
        if(state == this->_state.current)
            return;

        std::lock_guard<std::mutex> lockGuard(this->_stateMutex);
        this->_state.previous = this->_state.current;
        this->_state.current = state;
    }

    TunnelID LibhvWsClientTunnel::id() {
        return this->_tunnelID;
    }

    Tunnel::Type LibhvWsClientTunnel::type() {
        return Tunnel::RELIABLE;
    }

    size_t LibhvWsClientTunnel::send(const omg::Byte *payload, omg::size_t length) {
        if(length < 1) return -1;

        // TODO:长度包含了头
        if(this->_state.current != CONNECTED)
            return -1;

        return this->_webSocketClient->send(reinterpret_cast<const char*>(payload), length);
    }

    const LibhvWsClientTunnel::StateResult& LibhvWsClientTunnel::state() {
        return this->_state;
    }

    HANDLER_ID LibhvWsClientTunnel::addOnReadyHandler(omg::OnReadyCallback onReadyCallback) {
        return this->_onReadyCallbacks.add(onReadyCallback);
    }
    void LibhvWsClientTunnel::removeOnReadyHandler(HANDLER_ID handlerID) {
        this->_onReadyCallbacks.remove(handlerID);
    }

    HANDLER_ID LibhvWsClientTunnel::addOnErrorHandler(omg::OnErrorCallback onErrorCallback) {
        return this->_onErrorCallbacks.add(onErrorCallback);
    }
    void LibhvWsClientTunnel::removeOnErrorHandler(HANDLER_ID handlerID) {
        this->_onErrorCallbacks.remove(handlerID);
    }

    HANDLER_ID LibhvWsClientTunnel::addOnDestroyHandler(omg::OnDestroyCallback onDestroyCallback) {
        return this->_onDestroyCallbacks.add(onDestroyCallback);
    }
    void LibhvWsClientTunnel::removeOnDestroyHandler(HANDLER_ID handlerID) {
        this->_onDestroyCallbacks.remove(handlerID);
    }
}
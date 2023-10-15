//
// Created by Kris on 2023/10/5.
//

#include "LibhvWsClientTunnel.h"

#include <utility>

using WsClient = hv::WebSocketClient;
using WsClientUniquePtr = std::unique_ptr<WsClient>;

omg::LibhvWsClientTunnel::LibhvWsClientTunnel(hv::EventLoopPtr eventLoop, std::string  url, TunnelID tunnelID)
    : _state({INITIAL, INITIAL}),
      _tunnelID(tunnelID),
      _url(std::move(url)),
      _webSocketClient(WsClientUniquePtr(new WsClient(std::move(eventLoop))))
{
    this->_webSocketClient->onopen = [this](){
        this->changeState(CONNECTED);
    };

    this->_webSocketClient->onmessage = [this](const std::string& payload){

        if(!this->onReceive)
            return;

        const Byte* data = (const Byte*)payload.c_str();

        this->onReceive(shared_from_this(), data, payload.size());
    };

    this->_webSocketClient->onclose = [this](){

        // 如果上一步还是在连接中 那就是发生错误
        if(this->_state.current == CONNECTING){
            this->changeState(ERROR);
            return;
        }

        // 否则就是正常关闭

        // 为了实现状态切换 先调用一下
        this->changeState(DESTROYING);
        this->changeState(DESTROYED);
    };

    // 设置心跳间隔
    this->_webSocketClient->setPingInterval(1000*5);

    // 设置重连策略
    this->_reconnSetting.min_delay = 1000;
    this->_reconnSetting.max_delay = 1000 * 10;
    this->_reconnSetting.delay_policy = 2;
    this->_reconnSetting.max_retry_cnt = 20;
    this->_webSocketClient->setReconnect(&this->_reconnSetting);

    // 发起连接
    this->_webSocketClient->open(this->_url.c_str());

    // 切换连接中状态
    this->changeState(CONNECTING);
}
omg::LibhvWsClientTunnel::~LibhvWsClientTunnel() noexcept {
    this->cleanup();
}

void omg::LibhvWsClientTunnel::cleanup() {
    this->changeState(DESTROYING);
    this->_webSocketClient->close();
}

void omg::LibhvWsClientTunnel::changeState(omg::Tunnel::State state) {
    // 如果当前状态和要切换的状态一样就跳过
    if(state == this->_state.current)
        return;

    std::lock_guard<std::mutex> lockGuard(this->_locker);
    this->_state.previous = this->_state.current;
    this->_state.current = state;
}

omg::LibhvWsClientTunnel::Type omg::LibhvWsClientTunnel::type() {
    return Type::RELIABLE;
}

const omg::LibhvWsClientTunnel::StateResult& omg::LibhvWsClientTunnel::state() {
    return this->_state;
}

omg::TunnelID omg::LibhvWsClientTunnel::id() {
    return this->_tunnelID;
}

omg::size_t omg::LibhvWsClientTunnel::send(const omg::Byte* payload, size_t length) {
    return this->_webSocketClient->send((char*) payload, length);
}

int omg::LibhvWsClientTunnel::destroy() {
    this->cleanup();
    this->_webSocketClient->close();
    return 0;
}
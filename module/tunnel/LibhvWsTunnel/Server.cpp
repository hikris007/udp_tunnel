//
// Created by Kris on 2023/10/21.
//

#include "Server.h"

omg::LibhvWsServerTunnel::LibhvWsServerTunnel(omg::TunnelID tunnelID, WebSocketChannelPtr webSocketChannel)
    : _tunnelID(tunnelID),
      _state({CONNECTING, CONNECTED}),
      _webSocketChannel(webSocketChannel)
{

}

size_t omg::LibhvWsServerTunnel::send(const omg::Byte *payload, omg::size_t length) {
    if(payload == nullptr || length < 1)return -1;

    if(this->_webSocketChannel.expired())
        return -1;

    std::shared_ptr<hv::WebSocketChannel> webSocketChannel = this->_webSocketChannel.lock();

    // TODO: 头的长度
    return webSocketChannel->send(reinterpret_cast<const char*>(payload), length);
}

const omg::Tunnel::StateResult &omg::LibhvWsServerTunnel::state() {
    return this->_state;
}

omg::TunnelID omg::LibhvWsServerTunnel::id() {
    return this->_tunnelID;
}

void omg::LibhvWsServerTunnel::changeState(omg::Tunnel::State state) {
    // 如果当前状态和要切换的状态一样就跳过
    if(state == this->_state.current)
        return;

    std::lock_guard<std::mutex> lockGuard(this->_stateMutex);
    this->_state.previous = this->_state.current;
    this->_state.current = state;
}

omg::Tunnel::Type omg::LibhvWsServerTunnel::type() {
    return omg::Tunnel::RELIABLE;
}

int omg::LibhvWsServerTunnel::destroy() {
    if(this->_state.current != CONNECTED &&
        this->_state.current != CONNECTING &&
        this->_state.current != RECONNECTING
    )
        return 0;

    if(this->_webSocketChannel.expired())
        return -1;

    std::shared_ptr<hv::WebSocketChannel> webSocketChannel = this->_webSocketChannel.lock();

    if(webSocketChannel->isOpened())
        webSocketChannel->close();

    this->changeState(DESTROYING);
    this->changeState(DESTROYED);
    this->_onDestroyCallbacks.trigger(shared_from_this());
    return 0;
}

HANDLER_ID omg::LibhvWsServerTunnel::addOnReadyHandler(omg::OnReadyCallback onReadyCallback) {
    return -1;
}

void omg::LibhvWsServerTunnel::removeOnReadyHandler(HANDLER_ID handlerID) {}

HANDLER_ID omg::LibhvWsServerTunnel::addOnErrorHandler(omg::OnErrorCallback onErrorCallback) {
    return this->_onErrorCallbacks.add(onErrorCallback);
}
void omg::LibhvWsServerTunnel::removeOnErrorHandler(HANDLER_ID handlerID) {
    this->_onErrorCallbacks.remove(handlerID);
}

HANDLER_ID omg::LibhvWsServerTunnel::addOnDestroyHandler(omg::OnDestroyCallback onDestroyCallback) {
    return this->_onDestroyCallbacks.add(onDestroyCallback);
}
void omg::LibhvWsServerTunnel::removeOnDestroyHandler(HANDLER_ID handlerID) {
    this->_onDestroyCallbacks.remove(handlerID);
}
//
// Created by Kris Allen on 2023/9/18.
//

#include "TunnelFactory.h"

void omg::TunnelFactory::setEventLoopPtr(hv::EventLoopPtr eventLoop) {
    _eventLoop = std::move(eventLoop);
}

void omg::TunnelFactory::deleteEventLoopPtr() {
    _eventLoop = nullptr;
}

hv::EventLoopPtr omg::TunnelFactory::getEventLoopPtr() {
    return _eventLoop;
}

void omg::TunnelFactory::initIDPool() {
    size_t shortOfCount = this->_poolSize - this->_tunnelIDPool.size();
    if(shortOfCount <= 0)
        return;

    std::lock_guard<std::mutex> lockGuard(this->_poolLocker);
    for(
            size_t end = this->_lastTunnelID + this->_poolSize;
            this->_lastTunnelID <= end;
            this->_lastTunnelID++
    ){
        this->_tunnelIDPool.push(this->_lastTunnelID);
    }
}

omg::TunnelFactory &omg::TunnelFactory::getInstance() {
    static omg::TunnelFactory instance;
    return instance;
}

omg::TunnelFactory::TunnelFactory() {
    this->initIDPool();
}

omg::TunnelPtr omg::TunnelFactory::createTunnel(TransportProtocol transportProtocol, const std::string& endpoint) {
    switch (transportProtocol) {
        case Websocket:
            return createWsTunnel(endpoint);

        default:
            return nullptr;
    }
}

omg::TunnelID omg::TunnelFactory::getTunnelID() {
    if(this->_tunnelIDPool.empty())
        return INVALID_TUNNEL_ID;

    std::lock_guard<std::mutex> lockGuard(this->_poolLocker);
    TunnelID tunnelID = this->_tunnelIDPool.front();
    this->_tunnelIDPool.pop();
    return tunnelID;
}

void omg::TunnelFactory::putTunnelID(omg::TunnelID tunnelID) {
    std::lock_guard<std::mutex> lockGuard(this->_poolLocker);
    this->_tunnelIDPool.push(tunnelID);
}

omg::TunnelPtr omg::TunnelFactory::createWsTunnel(const std::string &endpoint) {
    TunnelID tunnelID = this->getTunnelID();
    if(tunnelID == INVALID_TUNNEL_ID) return nullptr;

    TunnelPtr tunnel = std::make_shared<LibhvWsClientTunnel>(_eventLoop, endpoint, tunnelID);
    if(tunnel == nullptr)return nullptr;

    // 注册回调

    // TODO: 考虑错误时怎么操作
    // 当关闭时回收ID
    tunnel->addOnDestroyHandler([this](const TunnelPtr& tunnel){
        this->putTunnelID(tunnel->id());
    });

    return tunnel;
}
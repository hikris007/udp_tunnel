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

omg::Tunnel* omg::TunnelFactory::createTunnel(TransportProtocol transportProtocol, const std::string& endpoint) {
    switch (transportProtocol) {
        case Websocket:
            return createWsTunnel(endpoint);

        default:
            return nullptr;
    }
}

omg::Tunnel *omg::TunnelFactory::createWsTunnel(const std::string &endpoint) {
    return new LibhvWsClientTunnel(_eventLoop, endpoint, 1);
}
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
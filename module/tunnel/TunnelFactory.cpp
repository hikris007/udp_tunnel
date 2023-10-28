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

omg::TunnelFactory &omg::TunnelFactory::getInstance() {
    static omg::TunnelFactory instance;
    return instance;
}

omg::TunnelFactory::TunnelFactory() {

}

omg::TunnelPtr omg::TunnelFactory::createTunnel(TransportProtocol transportProtocol, const std::string& endpoint) {

    // 如果 TunnelID 池 <=10/1 就重新生成
    if(this->_tunnelIDPool.poolCount() <= (this->_tunnelIDPool.poolSize() / 10))
        this->_tunnelIDPool.fillPool();

    switch (transportProtocol) {
        case Websocket:
            return createWsTunnel(endpoint);

        default:
            return nullptr;
    }
}

omg::TunnelPtr omg::TunnelFactory::createWsTunnel(const std::string &endpoint) {
    TunnelID tunnelID = this->_tunnelIDPool.getTunnelID();
    if(tunnelID == INVALID_TUNNEL_ID) return nullptr;

    TunnelPtr tunnel = std::make_shared<LibhvWsClientTunnel>(_eventLoop, endpoint, tunnelID);
    if(tunnel == nullptr)return nullptr;

    // 注册回调

    // 当失败时也是需要回收ID
    tunnel->addOnErrorHandler([this](const TunnelPtr& tunnel, void* data){
        this->_tunnelIDPool.putTunnelID(tunnel->id());
    });

    // 当关闭时回收ID
    tunnel->addOnDestroyHandler([this](const TunnelPtr& tunnel){
        this->_tunnelIDPool.putTunnelID(tunnel->id());
    });

    return tunnel;
}
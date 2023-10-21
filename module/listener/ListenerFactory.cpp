//
// Created by Kris on 2023/10/21.
//

#include "ListenerFactory.h"

omg::ListenerFactory::ListenerFactory() {

}

void omg::ListenerFactory::setEventLoopPtr(hv::EventLoopPtr eventLoop) {
    _eventLoop = std::move(eventLoop);
}

void omg::ListenerFactory::deleteEventLoopPtr() {
    _eventLoop = nullptr;
}

hv::EventLoopPtr omg::ListenerFactory::getEventLoopPtr() {
    return _eventLoop;
}

omg::ListenerFactory &omg::ListenerFactory::getInstance() {
    static omg::ListenerFactory instance;
    return instance;
}

int omg::ListenerFactory::createListener(
        omg::TransportProtocol transportProtocol,
        const std::string &listenDescription,
        omg::ListenerPtr &listener
        ) {
    switch (transportProtocol) {
        case Websocket:
            return this->createLibhvWsListener(listenDescription, listener);
    }

    return -1;
}

int omg::ListenerFactory::createLibhvWsListener(const std::string &listenDescription, omg::ListenerPtr &listener) {
    listener = std::make_shared<LibhvWsListener>(this->_eventLoop);
    return 0;
}
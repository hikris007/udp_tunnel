//
// Created by Kris Allen on 2023/10/2.
//

#include "Server.h"

omg::Server::Server(AppContext* appContext)
    : _appContext(appContext),
      _eventLoop(std::make_shared<hv::EventLoop>()),
      _serverPairManager(std::make_shared<ServerPairManager>(this->_appContext, this->_eventLoop))
{
    omg::ListenerFactory::getInstance().createListener(
            this->_appContext->transportProtocol,
            this->_appContext->serverConfig->listenDescription,
            this->_listener
    );
}

int omg::Server::shutdown() {
    if(!this->_isRunning) return -1;

    std::lock_guard<std::mutex> lockGuard(this->_shutdownMutex);
    this->_listener->stop();
    this->_isRunning = false;
    LOGGER_WARN("The server is shutdown.");
    return 0;
}

int omg::Server::run() {
    if(this->_isRunning) return -1;

    std::lock_guard<std::mutex> lockGuard(this->_runMutex);

    this->_listener->onAccept = [this](TunnelPtr tunnel){
        this->_serverPairManager->onTunnelOpen(std::move(tunnel));
    };

    this->_listener->start(this->_appContext->serverConfig->listenDescription);

    this->_isRunning = true;
    LOGGER_WARN("The server is running on {}.", this->_appContext->serverConfig->listenDescription);

    return 0;
}
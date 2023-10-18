//
// Created by Kris Allen on 2023/10/2.
//

#include "Server.h"

omg::Server::Server(AppContext* appContext) {
    this->_appContext = appContext;

    this->_serverPairManager = std::make_shared<ServerPairManager>(this->_eventLoop, this->_appContext);
}

// TODO:
int omg::Server::shutdown() {
    return 0;
}

// TODO:
int Server::run() {
    return 0;
}
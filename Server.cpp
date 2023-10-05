//
// Created by Kris Allen on 2023/10/2.
//

#include "header/Server.h"

Server::Server(AppContext* appContext) {
    this->_appContext = appContext;

    this->_serverPairManager = std::make_shared<ServerPairManager>(this->_eventLoop, this->_appContext);
}

// TODO:
Int Server::shutdown() {
    return 0;
}

// TODO:
Int Server::run() {
    return 0;
}
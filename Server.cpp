//
// Created by Kris Allen on 2023/10/2.
//

#include "header/Server.h"

Server::Server(ServerConfig *config) {
    this->_config = config;

    this->_serverPairManager = std::make_shared<ServerPairManager>();
}
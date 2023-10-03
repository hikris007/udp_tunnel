//
// Created by Kris Allen on 2023/10/2.
//

#ifndef UDP_TUNNEL_SERVER_H
#define UDP_TUNNEL_SERVER_H

#include "config.h"
#include "ServerPairManager.h"

class Server {
public:
    Server(ServerConfig* config);

    Int run();
    Int shutdown();

private:
    ServerConfig* _config = nullptr;
    std::shared_ptr<ServerPairManager> _serverPairManager = nullptr;
};


#endif //UDP_TUNNEL_SERVER_H

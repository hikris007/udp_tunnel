//
// Created by Kris Allen on 2023/10/2.
//

#ifndef UDP_TUNNEL_SERVER_H
#define UDP_TUNNEL_SERVER_H

#include "config.h"

class Server {
public:
    Server(Config* config);

    Int run();
    Int shutdown();
};


#endif //UDP_TUNNEL_SERVER_H

//
// Created by Kris Allen on 2023/9/18.
//

#ifndef UDP_TUNNEL_TUNNELFACTORY_H
#define UDP_TUNNEL_TUNNELFACTORY_H

#include "Tunnel.h"
#include "TestTunnel.h"

class TunnelFactory {
public:
    static Tunnel* createTunnel();
};


#endif //UDP_TUNNEL_TUNNELFACTORY_H

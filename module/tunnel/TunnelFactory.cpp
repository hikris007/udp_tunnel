//
// Created by Kris Allen on 2023/9/18.
//

#include "TunnelFactory.h"

Tunnel *TunnelFactory::createTunnel() {
    TestTunnel* tunnel = new TestTunnel;
    return tunnel;
}
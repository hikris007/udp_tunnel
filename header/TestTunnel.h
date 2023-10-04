//
// Created by Kris Allen on 2023/10/4.
//

#ifndef UDP_TUNNEL_TESTTUNNEL_H
#define UDP_TUNNEL_TESTTUNNEL_H
#include "Tunnel.h"

class TestTunnel : public Tunnel{
    SizeT send(const Byte *payload, SizeT len) override;
    TunnelID id() override;
    State state() override;
    Type type() override;
    Int destroy() override;
};


#endif //UDP_TUNNEL_TESTTUNNEL_H

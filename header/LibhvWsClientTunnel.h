//
// Created by Kris on 2023/10/5.
//

#ifndef UDP_TUNNEL_LIBHVWSTUNNEL_H
#define UDP_TUNNEL_LIBHVWSTUNNEL_H

#include "Tunnel.h"

class LibhvWsClientTunnel : public Tunnel {
    TunnelID id() override;
    Int destroy() override;
    SizeT send(const Byte *payload, SizeT len) override;
    State state() override;
    Type type() override;
};


#endif //UDP_TUNNEL_LIBHVWSTUNNEL_H

//
// Created by Kris on 2023/10/5.
//

#ifndef UDP_TUNNEL_LIBHVWSTUNNEL_H
#define UDP_TUNNEL_LIBHVWSTUNNEL_H

#include "hv/WebSocketClient.h"
#include "Tunnel.h"

class LibhvWsClientTunnel : public Tunnel, public std::enable_shared_from_this<LibhvWsClientTunnel>{
    LibhvWsClientTunnel(hv::EventLoopPtr eventLoop, std::string url, TunnelID tunnelID);

    TunnelID id() override;
    Int destroy() override;
    SizeT send(const Byte *payload, SizeT len) override;
    State state() override;
    Type type() override;

private:
    TunnelID _tunnelID = INVALID_TUNNEL_ID;
    std::string _url;
    std::unique_ptr<hv::WebSocketClient> _webSocketClient = nullptr;
    reconn_setting_t _reconnSetting;
};


#endif //UDP_TUNNEL_LIBHVWSTUNNEL_H

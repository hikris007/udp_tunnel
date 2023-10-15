//
// Created by Kris on 2023/10/5.
//

#ifndef UDP_TUNNEL_LIBHVWSTUNNEL_H
#define UDP_TUNNEL_LIBHVWSTUNNEL_H

#include "hv/WebSocketClient.h"
#include "../Tunnel.h"

namespace omg {
    class LibhvWsClientTunnel : public Tunnel, public std::enable_shared_from_this<LibhvWsClientTunnel>{

    public:
        LibhvWsClientTunnel(hv::EventLoopPtr eventLoop, std::string  url, TunnelID tunnelID);
        ~LibhvWsClientTunnel() override;

        TunnelID id() override;
        int destroy() override;
        size_t send(const Byte *payload, size_t len) override;
        const StateResult& state() override;
        Type type() override;

    protected:
        void changeState(State state);
        void cleanup();

    private:
        StateResult _state;

        TunnelID _tunnelID = INVALID_TUNNEL_ID;

        std::string _url;
        std::unique_ptr<hv::WebSocketClient> _webSocketClient = nullptr;
        reconn_setting_t _reconnSetting;
    };
}

#endif //UDP_TUNNEL_LIBHVWSTUNNEL_H

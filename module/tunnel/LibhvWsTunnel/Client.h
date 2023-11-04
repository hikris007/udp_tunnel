//
// Created by Kris Allen on 2023/10/17.
//

#ifndef UDP_TUNNEL_LIBHV_WS_CLIENT_H
#define UDP_TUNNEL_LIBHV_WS_CLIENT_H

#include "../Tunnel.h"
#include "WebSocketClient.h"

namespace omg {
    using WsClient = WebSocketClient;
    using WsClientUniquePtr = std::unique_ptr<WsClient>;

    class LibhvWsClientTunnel : public Tunnel, public std::enable_shared_from_this<LibhvWsClientTunnel>{
    public:
        LibhvWsClientTunnel(hv::EventLoopPtr eventLoop, std::string  url, TunnelID tunnelID);

        int destroy() override;
        size_t send(const omg::Byte *payload, omg::size_t len) override;
        const StateResult & state() override;
        Type type() override;
        TunnelID id() override;

        HANDLER_ID addOnReadyHandler(omg::OnReadyCallback onReadyCallback) override;
        void removeOnReadyHandler(HANDLER_ID handlerID) override;

        HANDLER_ID addOnErrorHandler(omg::OnErrorCallback onErrorCallback) override;
        void removeOnErrorHandler(HANDLER_ID handlerID) override;

        HANDLER_ID addOnDestroyHandler(omg::OnDestroyCallback onDestroyCallback) override;
        void removeOnDestroyHandler(HANDLER_ID handlerID) override;

    protected:
        void changeState(State state);

    private:
        StateResult _state;
        TunnelID _tunnelID;
        std::string _url;

        WsClientUniquePtr _webSocketClient = nullptr;
        reconn_setting_t _reconnSetting;

        CallBackManager<const TunnelPtr&> _onReadyCallbacks;
        CallBackManager<const TunnelPtr&, void*> _onErrorCallbacks;
        CallBackManager<const TunnelPtr&> _onDestroyCallbacks;

        std::mutex _stateMutex;

    };
}

#endif //UDP_TUNNEL_LIBHV_WS_CLIENT_H

//
// Created by Kris on 2023/10/21.
//

#ifndef UDP_TUNNEL_LIBHV_WS_SERVER_H
#define UDP_TUNNEL_LIBHV_WS_SERVER_H

#include "hv/WebSocketChannel.h"
#include "../Tunnel.h"

namespace omg {
class LibhvWsServerTunnel : public Tunnel, public std::enable_shared_from_this<LibhvWsServerTunnel>{
    public:
        LibhvWsServerTunnel(TunnelID tunnelID, WebSocketChannelPtr webSocketChannel);

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

        std::weak_ptr<hv::WebSocketChannel> _webSocketChannel;

        CallBackManager<const TunnelPtr&, void*> _onErrorCallbacks;
        CallBackManager<const TunnelPtr&> _onDestroyCallbacks;

        std::mutex _stateMutex;
    };
}

#endif //UDP_TUNNEL_LIBHV_WS_SERVER_H

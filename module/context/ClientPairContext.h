//
// Created by Kris on 2023/10/11.
//

#ifndef UDP_TUNNEL_CLIENTPAIRCONTEXT_H
#define UDP_TUNNEL_CLIENTPAIRCONTEXT_H

#include <memory>
#include "../tunnel/Tunnel.h"
#include "../../header/typedef.h"

namespace omg {
    class ClientPairContext {
    public:
        SizeT _lastDataReceivedTime = 0;
        SizeT _lastDataSentTime = 0;
        std::shared_ptr<ClientPairManager> _clientPairManagerPtr = nullptr;
        TunnelID _tunnelID = INVALID_TUNNEL_ID;
        std::string _sourceAddress;
        sockaddr* _sourceAddressSockAddr = nullptr;
    private:

    };

    typedef std::shared_ptr<ClientPairContext> ClientPairContextPtr;
}

#endif //UDP_TUNNEL_CLIENTPAIRCONTEXT_H

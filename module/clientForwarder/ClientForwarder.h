//
// Created by Kris Allen on 2023/9/28.
//

#ifndef UDP_TUNNEL_CLIENTFORWARDER_H
#define UDP_TUNNEL_CLIENTFORWARDER_H

#include <unordered_map>
#include <string>
#include "../clientPairManager/ClientPairManager.h"

namespace omg {
    class ClientForwarder {
    public:
        explicit ClientForwarder(std::shared_ptr<ClientPairManager> clientPairManager);

        // 接受到客户端数据
        SizeT onSend(const std::string& sourceAddress, Byte* payload, SizeT length);

        // 接收到响应数据
        std::function<SizeT(PairPtr pairPtr, Byte* payload, SizeT length)> onReceive = nullptr;
    protected:
    private:

        std::unordered_map<std::string, PairPtr> _sourceAddressMap;
        std::shared_ptr<ClientPairManager> _clientPairManager = nullptr;
    };
}

#endif //UDP_TUNNEL_CLIENTFORWARDER_H

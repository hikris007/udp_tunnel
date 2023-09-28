//
// Created by Kris Allen on 2023/9/28.
//

#ifndef UDP_TUNNEL_CLIENTFORWARDER_H
#define UDP_TUNNEL_CLIENTFORWARDER_H

#include <unordered_map>
#include <string>
#include "ClientPairManager.h"

class ClientForwarder {
public:
    // 接受到客户端数据
    SizeT onSend(const std::string, Byte* payload, SizeT length);

    // 接收到响应数据
    std::function<SizeT(PairPtr pairPtr, Byte* payload, SizeT length)> onReceive = nullptr;
protected:
private:

    std::unordered_map<std::string, PairPtr> _sourceAddressMap;
    std::shared_ptr<ClientPairManager> _clientPairManager;
};


#endif //UDP_TUNNEL_CLIENTFORWARDER_H

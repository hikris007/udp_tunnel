//
// Created by Kris on 2023/10/11.
//

#ifndef UDP_TUNNEL_SERVERPAIRCONTEXT_H
#define UDP_TUNNEL_SERVERPAIRCONTEXT_H

#include "hv/UdpClient.h"
#include "../tunnel/Tunnel.h"
#include "../pair/Pair.h"

namespace omg {
    class ServerPairContext {
    public:
        ServerPairContext(){
            // PairID 头大小 + UDP 包最大大小
            int capacity = sizeof(PairID) + 65535;
            this->_data = new Byte[capacity];
        }

        ~ServerPairContext(){
            delete []this->_data;
            this->_data = nullptr;
        }

        std::shared_ptr<hv::UdpClient> _udpClient = nullptr;
        TunnelPtr _tunnel = nullptr;
        Byte* _data = nullptr;
        std::mutex _dataMutex;
    };

    typedef std::shared_ptr<ServerPairContext> ServerPairContextPtr;
}


#endif //UDP_TUNNEL_SERVERPAIRCONTEXT_H

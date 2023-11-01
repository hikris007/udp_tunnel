//
// Created by Kris on 2023/10/11.
//

#ifndef UDP_TUNNEL_SERVERPAIRCONTEXT_H
#define UDP_TUNNEL_SERVERPAIRCONTEXT_H

#include "hv/UdpClient.h"
#include "../tunnel/Tunnel.h"
#include "../pair/Pair.h"
#include "../utils/time.hpp"
#include "../payload/Payload.h"

namespace omg {
    class ServerPairContext {
    public:
        ServerPairContext()
            : lastDataSentTime(utils::Time::GetCurrentTs()),
              lastDataReceivedTime(utils::Time::GetCurrentTs()),
              payload(sizeof(PairID) + 65535) // PairID 头大小 + UDP 包最大大小
        {

        }

        std::shared_ptr<hv::UdpClient> _udpClient = nullptr;
        TunnelPtr _tunnel = nullptr;

        /*!
         * Server 需要用来定时清理
         */
        size_t lastDataReceivedTime = 0; // 最后从服务端接收数据的时间戳（秒）
        size_t lastDataSentTime = 0; // 最后一次发送给服务端数据的时间戳（秒）

        Payload payload;
    };

    typedef std::shared_ptr<ServerPairContext> ServerPairContextPtr;
}


#endif //UDP_TUNNEL_SERVERPAIRCONTEXT_H

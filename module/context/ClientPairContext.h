//
// Created by Kris on 2023/10/11.
//

#ifndef UDP_TUNNEL_CLIENTPAIRCONTEXT_H
#define UDP_TUNNEL_CLIENTPAIRCONTEXT_H

#include <memory>
#include <sys/socket.h>
#include "../tunnel/Tunnel.h"
#include "../../header/typedef.h"
#include "../utils/time.hpp"

namespace omg {
    class ClientPairContext {
    public:
        ClientPairContext()
            : lastDataSentTime(utils::Time::GetCurrentTs()),
              lastDataReceivedTime(utils::Time::GetCurrentTs())
        {
            memset((void *) &this->sourceAddressSockAddr, 0, sizeof(this->sourceAddressSockAddr));
        }
        /*!
         * Client 需要用来定时清理
         */
        size_t lastDataReceivedTime = 0; // 最后从服务端接收数据的时间戳（秒）
        size_t lastDataSentTime = 0; // 最后一次发送给服务端数据的时间戳（秒）


        /*!
         * ClientForward 需要
         * 因为发送数据需要这种格式 所以多写一个这种格式的源地址
         */
        std::string sourceAddress; // 源地址 格式: ip:port
        sockaddr_u sourceAddressSockAddr{};

        TunnelPtr tunnel = nullptr; // Pair 所属的 隧道
    private:

    };

    typedef std::shared_ptr<ClientPairContext> ClientPairContextPtr;
}

#endif //UDP_TUNNEL_CLIENTPAIRCONTEXT_H

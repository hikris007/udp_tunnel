//
// Created by Kris Allen on 2023/9/28.
//

#ifndef UDP_TUNNEL_CLIENTFORWARDER_H
#define UDP_TUNNEL_CLIENTFORWARDER_H

#include <unordered_map>
#include <string>
#include "city.h"
#include "hv/hsocket.h"
#include "../clientPairManager/ClientPairManager.h"
#include "../utils/socket.hpp"
#include "../utils/time.hpp"

struct SockAddrHasher {
    size_t operator()(const sockaddr_u& addr) const {
        size_t hash = 0;
        omg::utils::Socket::GenerateSockAddrHash(addr, hash);
        return hash;
    }
};

struct SockAddrEqual {
    bool operator()(const sockaddr_u& lhs, const sockaddr_u& rhs) const {
        if (lhs.sa.sa_family != rhs.sa.sa_family) {
            return false;
        }

        switch (lhs.sa.sa_family) {
            case AF_INET:
                return lhs.sin.sin_port == rhs.sin.sin_port &&
                       lhs.sin.sin_addr.s_addr == rhs.sin.sin_addr.s_addr;

            case AF_INET6:
                return lhs.sin6.sin6_port == rhs.sin6.sin6_port &&
                       memcmp(&lhs.sin6.sin6_addr, &rhs.sin6.sin6_addr, sizeof(in6_addr)) == 0;

            default:
                return memcmp(&lhs.sa, &rhs.sa, sizeof(struct sockaddr)) == 0;
        }
    }
};

namespace omg {
    class ClientForwarder {
    public:
        explicit ClientForwarder(std::shared_ptr<ClientPairManager> clientPairManager);

        /*!
         * 处理来自客户端的数据(线程安全)
         * @param sourceAddress 源地址: ip:port
         * @param payload 数据
         * @param length 数据的长度
         * @return 成功发送的长度
         */
//        size_t onSend(const std::string& sourceAddress, const Byte* payload, size_t length);
        size_t onSend(const struct sockaddr* sourceAddress, const Byte* payload, size_t length);

        /*!
         * 处理从服务端发回来的数据(线程安全)
         * @param pair 接收到数据的 Pair
         * @param payload 接收到的数据
         * @param length 数据长度
         */
        std::function<size_t(const PairPtr& pair, const Byte* payload, size_t length)> onReceive = nullptr;
    protected:
    private:
        /*!
         * Pair 关闭的回调
         */
        std::function<void(const PairPtr& pair)> pairCloseHandler = nullptr;


        /*!
         * 一个源地址(sockaddr_u 的哈希) 对应一个 Pair
         */
//        std::unordered_map<sockaddr_u , PairPtr, SockAddrHasher, SockAddrEqual> _sourceAddressMap;
        std::unordered_map<size_t , PairPtr> _sourceAddressMap;

        /*!
         * ClientPairManager 的引用
         */
        std::shared_ptr<ClientPairManager> _clientPairManager = nullptr;

        std::mutex _locker;
    };
}

#endif //UDP_TUNNEL_CLIENTFORWARDER_H

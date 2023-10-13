//
// Created by Kris Allen on 2023/9/28.
//

#ifndef UDP_TUNNEL_CLIENTFORWARDER_H
#define UDP_TUNNEL_CLIENTFORWARDER_H

#include <unordered_map>
#include <string>
#include "hv/hsocket.h"
#include "../clientPairManager/ClientPairManager.h"
#include "../utils/socket.hpp"

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
        SizeT onSend(const std::string& sourceAddress, const Byte* payload, SizeT length);

        /*!
         * 处理从服务端发回来的数据(线程安全)
         * @param pair 接收到数据的 Pair
         * @param payload 接收到的数据
         * @param length 数据长度
         */
        std::function<SizeT(const PairPtr& pair, const Byte* payload, SizeT length)> onReceive = nullptr;
    protected:
    private:
        /*!
         * Pair 关闭的回调
         */
        std::function<void(const PairPtr& pair)> onPairClose = nullptr;

        /*!
         * 需要告诉 Pair 怎么发送数据
         */
        std::function<SizeT(const PairPtr& pair,const Byte* payload, SizeT length)> pairSendHandler = nullptr;

        /*!
         * 一个源地址 对应一个 Pair
         */
        std::unordered_map<std::string, PairPtr> _sourceAddressMap;

        /*!
         * ClientPairManager 的引用
         */
        std::shared_ptr<ClientPairManager> _clientPairManager = nullptr;

        std::mutex _locker;
    };
}

#endif //UDP_TUNNEL_CLIENTFORWARDER_H

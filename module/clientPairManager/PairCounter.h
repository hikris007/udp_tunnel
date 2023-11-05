//
// Created by Kris Allen on 2023/11/5.
//

#ifndef UDP_TUNNEL_PAIRCOUNTER_H
#define UDP_TUNNEL_PAIRCOUNTER_H

#include <unordered_map>
#include <list>
#include <mutex>

#include "../tunnel/Tunnel.h"

namespace omg {
    class PairCounter {
    public:
        explicit PairCounter(int tunnelCapacity);

        const std::unordered_map<TunnelID ,int>& counter() const;
        const std::list<TunnelID> availableTunnelIDList() const;

        /*!
         * 线程安全
         * @return 0 成功
         */
        int addTunnel(const TunnelID& tunnelID);

        /*!
         * 线程安全
         */
        TunnelID getAvailableTunnelID();

        /*!
         * 记录 Tunnel Pair 数量 线程安全
         * @param tunnelID
         * @param isIncrease
         */
        void pairCounterOperate(const TunnelID& tunnelID, bool isIncrease = true);

        int removeTunnel(const TunnelID& tunnelID);

    private:
        void pairCounterOperateWithoutLock(const TunnelID& tunnelID, bool isIncrease = true);

    private:
        int _tunnelCapacity = 0;
        std::mutex _locker; // 锁

        std::unordered_map<TunnelID ,int> _tunnelPairCounter; // Key 是传输层的ID 值是空闲数量
        std::list<TunnelID> _availableTunnelIDs; // 存放可用的（有空闲位置的）底层传输层ID
    };
}


#endif //UDP_TUNNEL_PAIRCOUNTER_H

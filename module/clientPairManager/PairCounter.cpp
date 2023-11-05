//
// Created by Kris Allen on 2023/11/5.
//

#include "PairCounter.h"

namespace omg {
    PairCounter::PairCounter(int tunnelCapacity)
        : _tunnelCapacity(tunnelCapacity)
    {

    }

    const std::unordered_map<TunnelID, int> &PairCounter::counter() const {
        return this->_tunnelPairCounter;
    }

    const std::list<TunnelID> PairCounter::availableTunnelIDList() const {
        return this->_availableTunnelIDs;
    }

    int PairCounter::addTunnel(const TunnelID& tunnelID) {
        std::lock_guard<std::mutex> lockGuard(this->_locker);

        // 添加到计数列表 & 添加到可用隧道列表
        this->_tunnelPairCounter.insert({tunnelID, 0});
        this->_availableTunnelIDs.push_back(tunnelID);
        return 0;
    }

    TunnelID PairCounter::getAvailableTunnelID() {
        std::lock_guard<std::mutex> lockGuard(this->_locker);

        if(this->_availableTunnelIDs.empty())
            return INVALID_TUNNEL_ID;

        TunnelID tunnelID = this->_availableTunnelIDs.front();
        this->pairCounterOperateWithoutLock(tunnelID, true);

        return tunnelID;
    }

    void PairCounter::pairCounterOperate(const omg::TunnelID &tunnelID, bool isIncrease) {
        std::lock_guard<std::mutex> lockGuard(this->_locker);
        this->pairCounterOperateWithoutLock(tunnelID, isIncrease);
    }

    void PairCounter::pairCounterOperateWithoutLock(const TunnelID& tunnelID, bool isIncrease) {

        // 在计数器中查找 & 因为要使用这个底层传输 所以计数+1
        auto targetCounter = this->_tunnelPairCounter.find(tunnelID);
        if(targetCounter == this->_tunnelPairCounter.end())return;

        if(isIncrease){
            targetCounter->second++;

            // 如果隧道的空位满了就从可用列表中移除
            if(targetCounter->second >= this->_tunnelCapacity){
                this->_availableTunnelIDs.pop_front();
            }
        }else{
            if(targetCounter->second > 0)
                targetCounter->second--;

            // 如果隧道有空位就添加到可用列表
            if(targetCounter->second < this->_tunnelCapacity){
                this->_availableTunnelIDs.push_back(tunnelID);
            }
        }
    }

    int PairCounter::removeTunnel(const TunnelID& tunnelID) {
        // 从本地 Tunnels 移除
        std::lock_guard<std::mutex> lockGuard(this->_locker);

        auto iterator = std::find(this->_availableTunnelIDs.begin(), this->_availableTunnelIDs.end(), tunnelID);
        if(iterator != this->_availableTunnelIDs.end())
            this->_availableTunnelIDs.erase(iterator);

        auto iterator2 = this->_tunnelPairCounter.find(tunnelID);
        if(iterator2 != this->_tunnelPairCounter.end())
            this->_tunnelPairCounter.erase(iterator2);

        return 0;
    }
}
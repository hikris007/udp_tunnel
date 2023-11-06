//
// Created by Kris Allen on 2023/10/28.
//

#include "TunnelIDPool.h"

omg::TunnelIDPool::TunnelIDPool(omg::size_t poolSize) : _poolSize(poolSize) {
    this->fillPool();
}

size_t omg::TunnelIDPool::poolCount() {
    return this->_tunnelIDPool.size();
}

size_t omg::TunnelIDPool::poolSize() {
    return this->_poolSize;
}

int omg::TunnelIDPool::fillPool() {
    size_t shortOfCount = this->_poolSize - this->_tunnelIDPool.size();
    if(shortOfCount <= 0)
        return shortOfCount;

    std::lock_guard<std::mutex> lockGuard(this->_poolLocker);
    for(
            size_t end = this->_lastTunnelID + this->_poolSize;
            this->_lastTunnelID <= end;
            this->_lastTunnelID++
            ){
        this->_tunnelIDPool.push(this->_lastTunnelID);
    }

    return shortOfCount;
}

omg::TunnelID omg::TunnelIDPool::getTunnelID() {
    if(this->_tunnelIDPool.empty())
        return INVALID_TUNNEL_ID;

    std::lock_guard<std::mutex> lockGuard(this->_poolLocker);
    TunnelID tunnelID = this->_tunnelIDPool.front();
    this->_tunnelIDPool.pop();
    return tunnelID;
}

void omg::TunnelIDPool::putTunnelID(omg::TunnelID tunnelID) {
    std::lock_guard<std::mutex> lockGuard(this->_poolLocker);
    this->_tunnelIDPool.push(tunnelID);
}
//
// Created by Kris on 2023/10/11.
//

#ifndef UDP_TUNNEL_SERVERTUNNELCONTEXT_H
#define UDP_TUNNEL_SERVERTUNNELCONTEXT_H

#include "../pair/Pair.h"

namespace omg {
    class ServerTunnelContext {
    public:
        void addPair(PairPtr pair){
            std::lock_guard<std::mutex> locker(this->_locker);
            this->_pairs.insert({ pair->id(), pair });
        }

        void removePair(const PairPtr& pair){
            std::lock_guard<std::mutex> locker(this->_locker);
            auto iterator = this->_pairs.find(pair->id());
            if(iterator == this->_pairs.end())
                return;

            this->_pairs.erase(iterator);
        }

        // 根据 PairID 获取 Pair
        // 如果不存在 / 引用过期 都返回 nullptr
        PairPtr getPair(PairID pairID){
            if(pairID == INVALID_PAIR_ID)
                return nullptr;

            std::lock_guard<std::mutex> locker(this->_locker);

            auto iterator = this->_pairs.find(pairID);
            if (iterator == this->_pairs.end())
                return nullptr;

            if(iterator->second.expired())
                return nullptr;

            PairPtr pairPtr = iterator->second.lock();

            return std::move(pairPtr);
        }
    private:
        std::mutex _locker;
        std::unordered_map<PairID, std::weak_ptr<Pair>> _pairs; // 映射的集合
    };

    typedef std::shared_ptr<ServerTunnelContext> ServerTunnelContextPtr;
}




#endif //UDP_TUNNEL_SERVERTUNNELCONTEXT_H

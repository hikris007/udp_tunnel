//
// Created by Kris on 2023/10/11.
//

#ifndef UDP_TUNNEL_CLIENTTUNNELCONTEXT_H
#define UDP_TUNNEL_CLIENTTUNNELCONTEXT_H

#include <unordered_set>

namespace omg {
    class ClientTunnelContext {

    public:
        ClientTunnelContext(const TunnelPtr& tunnel, size_t carryingCapacity)
            : _carryingCapacity( carryingCapacity),
              _tunnel(tunnel)
        {
            // 初始化 PairID 池
            this->initAvailablePairIDs();
        }

        size_t vacancy() const {
            return this->_availablePairIDs.size();
        }

        size_t capacity() const {
            return this->_carryingCapacity;
        }

        /*!
         * 从隧道里获取序号ID
         * @return 返回序号id，失败则返回 INVALID_PAIR_ID
         */
        PairID takeSeatNumber(){
            std::lock_guard<std::mutex> lockGuard(this->_availablePairIDsLocker);
            if(this->_availablePairIDs.empty())
                return INVALID_PAIR_ID;

            auto iterator = this->_availablePairIDs.begin();
            PairID pairID = *iterator;
            this->_availablePairIDs.erase(iterator);

            return pairID;
        }

        /*!
         * 放回序号ID
         * @param pairID
         */
        void putSeatNumber(PairID pairID){
            std::lock_guard<std::mutex> lockGuard(this->_availablePairIDsLocker);
            this->_availablePairIDs.insert(pairID);
        }

        int bindPair(PairPtr& pair){
            // 关联隧道
            this->addPair(pair);

            pair->addOnCloseHandler([this](const PairPtr& pair){

                // 解除关联 & 释放 PairID
                this->removePair(pair);
                this->putSeatNumber(pair->id());

                if(_tunnel.expired()){
                    LOGGER_INFO("Pair (id: {}) is release seat number: {}, owner tunnel wake pointer is expired", pair->id(), pair->id());
                    return;
                }

                TunnelPtr tunnel = this->_tunnel.lock();
                LOGGER_INFO("Pair (id: {}) is release seat number: {}, owner tunnel (id: {})", pair->id(), pair->id(), tunnel->id());
            });
            return 0;
        }

        /*!
         * 获取Pair根据PairID
         * @param pairID 需要查找的ID
         * @param pairPtr 返回的结果
         */
        void getPair(PairID pairID, PairPtr& pairPtr){
            if(pairID == INVALID_PAIR_ID)
                return;

            auto iterator = this->_pairs.find(pairID);
            if(iterator == this->_pairs.end())
                return;

            if(iterator->second.expired())
                return;

            pairPtr = iterator->second.lock();
        }



        void foreachPairs(const std::function<void(PairPtr &)>& handler) {
            std::lock_guard<std::mutex> lockGuard(this->_pairsLocker);

            for(auto & iterator : this->_pairs){
                if(iterator.second.expired())
                    return;

                PairPtr pair = iterator.second.lock();
                handler(pair);
            }
        }

    private:
        void addPair(PairPtr pair){
            std::lock_guard<std::mutex> lockGuard(this->_pairsLocker);
            this->_pairs.insert({ pair->id(), std::move(pair) });
        }

        /*!
         * 删除Pair和隧道的关联
         * @param pair
         */
        void removePair(const PairPtr& pair){
            std::lock_guard<std::mutex> lockGuard(this->_pairsLocker);
            auto iterator = this->_pairs.find(pair->id());
            if(iterator == this->_pairs.end())
                return;

            this->_pairs.erase(iterator);
        }

        void initAvailablePairIDs(){
            size_t begin = INVALID_PAIR_ID+1;

            for(size_t i = begin; this->_availablePairIDs.size() < this->_carryingCapacity; i++){
                this->putSeatNumber(i);
            }
        }

    private:
        std::weak_ptr<Tunnel> _tunnel;
        std::unordered_map<PairID, std::weak_ptr<Pair>> _pairs; // 映射的集合
        std::unordered_set<PairID> _availablePairIDs; // 可用的映射 ID
        std::mutex _availablePairIDsLocker;
        std::mutex _pairsLocker;
        size_t _carryingCapacity;
    };

    typedef std::shared_ptr<ClientTunnelContext> ClientTunnelContextPtr;
}

#endif //UDP_TUNNEL_CLIENTTUNNELCONTEXT_H

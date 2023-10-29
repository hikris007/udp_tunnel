//
// Created by Kris on 2023/10/11.
//

#ifndef UDP_TUNNEL_CLIENTTUNNELCONTEXT_H
#define UDP_TUNNEL_CLIENTTUNNELCONTEXT_H

#include <unordered_set>

namespace omg {
    class ClientTunnelContext {

    public:
        ClientTunnelContext(size_t carryingCapacity){
            // 初始化 PairID 池
            this->initAvailablePairIDs(carryingCapacity);
        }

        /*!
         * 从隧道里获取序号ID
         * @return 返回序号id，失败则返回 INVALID_PAIR_ID
         */
        PairID takeSeatNumber(){
            std::lock_guard<std::mutex> locker(this->_locker);
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
            std::lock_guard<std::mutex> locker(this->_locker);
            this->_availablePairIDs.insert(pairID);
        }


        void addPair(PairPtr pair){
            std::lock_guard<std::mutex> locker(this->_locker);
            this->_pairs.insert({ pair->id(), std::move(pair) });
        }

        /*!
         * 获取Pair根据PairID
         * @param pairID 需要查找的ID
         * @param pairPtr 返回的结果
         */
        void getPair(PairID pairID, PairPtr& pairPtr){
            if(pairID == INVALID_PAIR_ID)
                return;

            std::lock_guard<std::mutex> locker(this->_locker);

            auto iterator = this->_pairs.find(pairID);
            if(iterator == this->_pairs.end())
                return;

            if(iterator->second.expired())
                return;

            pairPtr = iterator->second.lock();
        }

        /*!
         * 删除Pair和隧道的关联
         * @param pair
         */
        void removePair(const PairPtr& pair){
            std::lock_guard<std::mutex> locker(this->_locker);
            auto iterator = this->_pairs.find(pair->id());
            if(iterator == this->_pairs.end())
                return;

            this->_pairs.erase(iterator);
        }

        void foreachPairs(const std::function<void(PairPtr &)>& handler) {
            std::lock_guard<std::mutex> lockGuard(this->_locker);

            for(auto & iterator : this->_pairs){
                if(iterator.second.expired())
                    return;

                PairPtr pair = iterator.second.lock();
                handler(pair);
            }
        }

    private:
        void initAvailablePairIDs(size_t carryingCapacity){
            size_t begin = INVALID_PAIR_ID+1;

            for(size_t i = begin; this->_availablePairIDs.size() < carryingCapacity; i++){
                this->putSeatNumber(i);
            }
        }

    private:
        std::unordered_map<PairID, std::weak_ptr<Pair>> _pairs; // 映射的集合
        std::unordered_set<PairID> _availablePairIDs; // 可用的映射 ID
        std::mutex _locker;
    };

    typedef std::shared_ptr<ClientTunnelContext> ClientTunnelContextPtr;
}

#endif //UDP_TUNNEL_CLIENTTUNNELCONTEXT_H

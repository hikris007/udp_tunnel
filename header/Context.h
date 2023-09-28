//
// Created by Kris Allen on 2023/9/18.
//

#ifndef UDP_TUNNEL_CONTEXT_H
#define UDP_TUNNEL_CONTEXT_H

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "ClientPairManager.h"

class ClientPairContext {
public:
    std::weak_ptr<ClientPairManager> _clientPairManagerPtr;
    std::weak_ptr<Tunnel> _tunnel;
    std::string _sourceAddress;
private:

};

class ServerPairContext {

};

class ClientTunnelContext {

public:
    ClientTunnelContext(SizeT carryingCapacity){
        // 初始化 PairID 池
        this->initAvailablePairIDs(carryingCapacity);
    }

    PairID get(){
        std::lock_guard<std::mutex> locker(this->_locker);
        if(this->_availablePairIDs.empty())
            return INVALID_PAIR_ID;

        auto iterator = this->_availablePairIDs.begin();
        PairID pairID = *iterator;
        this->_availablePairIDs.erase(iterator);

        return pairID;
    }

    void put(PairID pairID){
        std::lock_guard<std::mutex> locker(this->_locker);
        this->_availablePairIDs.insert(pairID);
    }

    void addPair(PairPtr pairPtr){
        std::lock_guard<std::mutex> locker(this->_locker);
        this->_pairs.insert({ pairPtr->id(), pairPtr });
    }

    void removePair(PairPtr pairPtr){
        std::lock_guard<std::mutex> locker(this->_locker);
        auto iterator = this->_pairs.find(pairPtr->id());
        if(iterator == this->_pairs.end())
            return;

        this->_pairs.erase(iterator);
    }

private:
    void initAvailablePairIDs(SizeT carryingCapacity){
        SizeT begin = INVALID_PAIR_ID+1;

        for(SizeT i = begin; this->_availablePairIDs.size() < carryingCapacity; i++){
            this->put(i);
        }
    }

private:
    std::unordered_map<PairID, std::weak_ptr<Pair>> _pairs; // 映射的集合
    std::unordered_set<PairID> _availablePairIDs; // 可用的映射 ID
    std::mutex _locker;
};

class ServerTunnelContext {

};

typedef std::shared_ptr<ClientPairContext> ClientPairContextPtr;
typedef std::shared_ptr<ClientTunnelContext> ClientTunnelContextPtr;

typedef std::shared_ptr<ServerPairContext> ServerPairContextPtr;
typedef std::shared_ptr<ServerTunnelContext> ServerTunnelContextPtr;

#endif //UDP_TUNNEL_CONTEXT_H

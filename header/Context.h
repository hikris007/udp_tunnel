//
// Created by Kris Allen on 2023/9/18.
//

#ifndef UDP_TUNNEL_CONTEXT_H
#define UDP_TUNNEL_CONTEXT_H

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "hv/UdpClient.h"
#include "netinet/in.h"
#include "ClientPairManager.h"

class ClientPairContext {
public:
    long long lastDataReceivedTime = 0;
    long long lastDataSentTime = 0;
    std::shared_ptr<ClientPairManager> _clientPairManagerPtr = nullptr;
    TunnelID _tunnelID = INVALID_TUNNEL_ID;
    std::string _sourceAddress;
    sockaddr* _sourceAddressSockAddr = nullptr;
private:

};

class ServerPairContext {
public:
    std::shared_ptr<hv::UdpClient> udpClient = nullptr;
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

    void addPair(PairPtr pair){
        std::lock_guard<std::mutex> locker(this->_locker);
        this->_pairs.insert({ pair->id(), std::move(pair) });
    }

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

    void removePair(const PairPtr& pair){
        std::lock_guard<std::mutex> locker(this->_locker);
        auto iterator = this->_pairs.find(pair->id());
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

typedef std::shared_ptr<ClientPairContext> ClientPairContextPtr;
typedef std::shared_ptr<ClientTunnelContext> ClientTunnelContextPtr;

typedef std::shared_ptr<ServerPairContext> ServerPairContextPtr;
typedef std::shared_ptr<ServerTunnelContext> ServerTunnelContextPtr;

#endif //UDP_TUNNEL_CONTEXT_H

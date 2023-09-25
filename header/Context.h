//
// Created by Kris Allen on 2023/9/18.
//

#ifndef UDP_TUNNEL_CONTEXT_H
#define UDP_TUNNEL_CONTEXT_H

#include <memory>
#include "ClientPairManager.h"

class ClientPairContext {
public:
    std::weak_ptr<ClientPairManager> _clientPairManagerPtr;
    std::weak_ptr<Tunnel> _tunnel;

private:

};

class ServerPairContext {

};

class ClientTunnelContext {

public:
    PairID get(){
        std::lock_guard<std::mutex> locker(this->_locker);
        if(this->_availablePairIDs.empty())
            return INVALID_PAIR_ID;

        PairID pairID = this->_availablePairIDs.front();
        this->_availablePairIDs.pop();

        return pairID;
    }

    void put(PairID pairID){
        std::lock_guard<std::mutex> locker(this->_locker);
        this->_availablePairIDs.push(pairID);
    }

    void addPair(std::weak_ptr<Pair> pair){
    }

    void removePair(std::weak_ptr<Pair> pairPtr){

    }

private:
    std::list<std::weak_ptr<Pair>> _pairs; // 映射的集合
    std::queue<PairID> _availablePairIDs; // 可用的映射 ID
    std::mutex _locker;
};

class ServerTunnelContext {

};

typedef std::shared_ptr<ClientPairContext> ClientPairContextPtr;
typedef std::shared_ptr<ClientTunnelContext> ClientTunnelContextPtr;

typedef std::shared_ptr<ServerPairContext> ServerPairContextPtr;
typedef std::shared_ptr<ServerTunnelContext> ServerTunnelContextPtr;

#endif //UDP_TUNNEL_CONTEXT_H

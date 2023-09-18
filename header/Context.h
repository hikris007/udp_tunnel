//
// Created by Kris Allen on 2023/9/18.
//

#ifndef UDP_TUNNEL_CONTEXT_H
#define UDP_TUNNEL_CONTEXT_H

#include <memory>
#include "ClientPairManager.h"

class ClientPairContext {
private:
    std::weak_ptr<ClientPairManager> clientPairManagerPtr_;
    std::weak_ptr<Tunnel> tunnel_;
};

class ServerPairContext {

};

class ClientTunnelContext {

public:
    PairID get(){
        std::lock_guard<std::mutex> locker(this->locker_);
        if(this->availablePairIDs_.empty())
            return INVALID_PAIR_ID;

        PairID pairID = this->availablePairIDs_.front();
        this->availablePairIDs_.pop();

        return pairID;
    }

    void put(PairID pairID){
        std::lock_guard<std::mutex> locker(this->locker_);
        this->availablePairIDs_.push(pairID);
    }

private:
    std::queue<PairID> availablePairIDs_;
    std::mutex locker_;
};

class ServerTunnelContext {

};

#endif //UDP_TUNNEL_CONTEXT_H

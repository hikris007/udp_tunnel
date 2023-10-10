//
// Created by Kris on 2023/9/14.
//

#ifndef UDP_TUNNEL_CALLBACKMANAGER_H
#define UDP_TUNNEL_CALLBACKMANAGER_H

#include <list>
#include <map>
#include <mutex>
#include <functional>

#include "../../header/typedef.h"

typedef omg::Uint32 HANDLER_ID;

namespace omg {
    template<typename ... Args>
    class CallBackManager {
    public:

        using Callback = std::function<void(Args...)>;
        using Callbacks = std::list<Callback>;

        HANDLER_ID add(Callback callback){
            std::lock_guard<std::mutex> lockGuard(this->locker_);

            HANDLER_ID handlerID = this->nextID++;
            typename Callbacks::iterator iterator = this->callbacks_.insert(this->callbacks_.end(), callback);
            this->idToIterator.insert(std::pair<HANDLER_ID, typename Callbacks::iterator>(handlerID, iterator));

            return handlerID;
        }

        void remove(HANDLER_ID handlerID){
            std::lock_guard<std::mutex> lockGuard(this->locker_);

            typename std::map<HANDLER_ID, typename Callbacks::iterator>::iterator iterator = this->idToIterator.find(handlerID);
            if(iterator == this->idToIterator.end())
                return;

            this->callbacks_.erase(iterator->second);
            this->idToIterator.erase(iterator);
        }

        void trigger(Args ...args){
            for(auto& callback: this->callbacks_){
                callback(args ...);
            }
        }
    private:
        Callbacks callbacks_;
        std::map<HANDLER_ID, typename Callbacks::iterator> idToIterator;
        std::mutex locker_;
        HANDLER_ID nextID = 0;
    };
}

#endif //UDP_TUNNEL_CALLBACKMANAGER_H

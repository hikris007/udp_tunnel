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
            std::lock_guard<std::mutex> lockGuard(this->_locker);

            HANDLER_ID handlerID = this->_nextID++;
            typename Callbacks::iterator iterator = this->_callbacks.insert(this->_callbacks.end(), callback);
            this->idToIterator.insert({handlerID, iterator});

            return handlerID;
        }

        void remove(HANDLER_ID handlerID){
            std::lock_guard<std::mutex> lockGuard(this->_locker);

            auto iterator = this->_idToIterator.find(handlerID);
            if(iterator == this->idToIterator.end())
                return;

            this->_callbacks.erase(iterator->second);
            this->_idToIterator.erase(iterator);
        }

        void trigger(Args ...args){
            for(auto iterator = this->_callbacks.rbegin(); iterator != this->_callbacks.rend(); iterator++){
                *callback(args ...);
            }
        }
    private:
        Callbacks _callbacks;
        std::map<HANDLER_ID, typename Callbacks::iterator> _idToIterator;
        std::mutex _locker;
        HANDLER_ID _nextID = 0;
    };
}

#endif //UDP_TUNNEL_CALLBACKMANAGER_H

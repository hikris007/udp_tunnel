//
// Created by Kris on 2023/11/6.
//

#include "IdleHandler.h"

omg::IdleHandler::IdleHandler() {}

HANDLER_ID omg::IdleHandler::addHandler(omg::IdleCallback callback) {
    this->_callBackManager.add(std::move(callback));
}

void omg::IdleHandler::removeHandler(HANDLER_ID handlerID) {
    this->_callBackManager.remove(handlerID);
}

omg::IdleHandler &omg::IdleHandler::getInstance() {
    static IdleHandler instance;
    return instance;
}

void omg::IdleHandler::libhvOnIdleTrigger(hidle_t *idle) {
    IdleHandler::getInstance()._callBackManager.trigger(nullptr);
}

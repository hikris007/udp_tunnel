//
// Created by Kris Allen on 2023/8/15.
//

#include "header/Pair.h"

void *Pair::context() {
    return this->_ctx;
}

template<class T> T* Pair::getContext() {
    std::lock_guard<std::mutex> locker(this->_locker);

    return (T*)this->_ctx;
}

void Pair::setContext(void *ctx) {
    std::lock_guard<std::mutex> locker(this->_locker);

    this->_ctx = ctx;
}

template<class T>
void Pair::deleteContext() {
    std::lock_guard<std::mutex> locker(this->_locker);

    if(this->_ctx == nullptr)
        return;

    delete (T*)this->_ctx;
    this->_ctx = nullptr;
}

template<class T>
std::shared_ptr<T> Pair::getContextPtr() {
    std::lock_guard<std::mutex> locker(this->_locker);

    return std::static_pointer_cast<T>(this->_ctxPtr);
}

void Pair::setContextPtr(const std::shared_ptr<void> &ctx) {
    std::lock_guard<std::mutex> locker(this->_locker);

    this->_ctxPtr = ctx;
}

void Pair::deleteContextPtr() {
    std::lock_guard<std::mutex> locker(this->_locker);

    this->_ctxPtr.reset();
}

PairID Pair::id() const {
    return this->_pairID;
}

SizeT Pair::send(const Byte *payload, SizeT len) {
    std::lock_guard<std::mutex> locker(this->_locker);

    if(this->handleSend == nullptr)
        return 0;

    return this->handleSend(shared_from_this(), payload, len);
}

HANDLER_ID Pair::addOnCloseHandler(onCloseCallback callback) {
    return this->onCloseCallbacks.add(callback);
}

void Pair::removeOnCloseHandler(HANDLER_ID handlerID) {
    this->onCloseCallbacks.remove(handlerID);
}

Int Pair::close() {
    std::lock_guard<std::mutex> locker(this->_locker);

    this->onCloseCallbacks.trigger(shared_from_this());

    return 0;
}
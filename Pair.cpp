//
// Created by Kris Allen on 2023/8/15.
//

#include "header/Pair.h"

Pair::Pair(PairID pairID) {
    this->_pairID = pairID;

    // PairID 头大小 + UDP 包最大大小
    int capacity = sizeof(PairID) + 65535;
    this->_data = new Byte[capacity];

    // 设置 PairID 头
    memcpy(this->_data, &this->_pairID, sizeof(PairID));
}

Pair::~Pair() {
    if(this->_data)
        delete []this->_data;
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

    memcpy(
           this->_data + sizeof(PairID),
           payload,
           len
    );

    return this->handleSend(shared_from_this(), this->_data, sizeof(PairID) + len);
}

HANDLER_ID Pair::addOnCloseHandler(onCloseCallback callback) {
    return this->_onCloseCallbacks.add(callback);
}

void Pair::removeOnCloseHandler(HANDLER_ID handlerID) {
    this->_onCloseCallbacks.remove(handlerID);
}

Int Pair::close() {
    std::lock_guard<std::mutex> locker(this->_locker);
    this->_onCloseCallbacks.trigger(shared_from_this());
    return 0;
}
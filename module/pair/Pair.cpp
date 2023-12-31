#include "Pair.h"

omg::Pair::Pair(PairID pairID) {
    this->_isClosed = false;
    this->_pairID = pairID;

    // PairID 头大小 + UDP 包最大大小
    int capacity = sizeof(PairID) + 65535;
    this->_data = new Byte[capacity];

    // 设置 PairID 头
    memcpy(this->_data, &this->_pairID, sizeof(PairID));
}

omg::Pair::~Pair() {
    delete []this->_data;
    this->_data = nullptr;
}

omg::PairID omg::Pair::id() const {
    return this->_pairID;
}

omg::size_t omg::Pair::send(const Byte *payload, size_t length) {
    std::lock_guard<std::mutex> lockGuard(this->_sendMutex);

    if(this->_isClosed)
        return -1;

    if(this->sendHandler == nullptr)
        return -1;

    memcpy(
           this->_data + sizeof(PairID),
           payload,
           length
    );

    return this->sendHandler(shared_from_this(), this->_data, sizeof(PairID) + length);
}

HANDLER_ID omg::Pair::addOnCloseHandler(onCloseCallback callback) {
    return this->_onCloseCallbacks.add(std::move(callback));
}

void omg::Pair::removeOnCloseHandler(HANDLER_ID handlerID) {
    this->_onCloseCallbacks.remove(handlerID);
}

int omg::Pair::close() {
    std::lock_guard<std::mutex> lockGuard(this->_closeMutex);
    if(this->_isClosed)
        return -1;

    this->_onCloseCallbacks.trigger(shared_from_this());
    this->_isClosed = true;

    return 0;
}
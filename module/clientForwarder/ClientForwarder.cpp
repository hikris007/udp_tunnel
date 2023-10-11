//
// Created by Kris Allen on 2023/9/28.
//

#include "ClientForwarder.h"

omg::ClientForwarder::ClientForwarder(std::shared_ptr<ClientPairManager> clientPairManager) {
    this->_clientPairManager = std::move(clientPairManager);
}

omg::SizeT omg::ClientForwarder::onSend(const std::string& sourceAddress, Byte *payload, SizeT length) {
    std::lock_guard<std::mutex> lockGuard(this->_locker);

    // 获取源地址对应的 Pair
    // 如果此来源地址没有对应的 Pair 则分配一个
    PairPtr pair = nullptr;
    auto iterator = this->_sourceAddressMap.find(sourceAddress);
    if(iterator == this->_sourceAddressMap.end()){
        this->_clientPairManager->createPair(pair);
    }else{
        pair = iterator->second;
    }

    // 获取 Pair 上下文
    ClientPairContextPtr clientPairContext = pair->getContextPtr<ClientPairContext>();

    // 如果是新添加的 Pair 则设置一些信息
    if(iterator == this->_sourceAddressMap.end()){
        clientPairContext->_sourceAddress = sourceAddress;
        clientPairContext->_sourceAddressSockAddr = nullptr;

        this->_sourceAddressMap.insert({ sourceAddress, pair });
        LOGGER_INFO("New pair:{} <----> {}", pair->id(), sourceAddress);
    }

    // 写入数据
    return pair->send(payload, length);
}
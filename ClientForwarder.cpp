//
// Created by Kris Allen on 2023/9/28.
//

#include "header/ClientForwarder.h"

SizeT ClientForwarder::onSend(const std::string sourceAddress, Byte *payload, SizeT length) {
    PairPtr pairPtr = nullptr;

    // 如果此来源地址没有对应的 Pair 则分配一个
    auto iterator = this->_sourceAddressMap.find(sourceAddress);
    if(iterator == this->_sourceAddressMap.end()){
        this->_clientPairManager->createPair(pairPtr);
    }else{
        pairPtr = iterator->second;
    }

    // 获取上下文
    ClientPairContextPtr clientPairContext = pairPtr->getContextPtr<ClientPairContext>();

    // 如果是新添加的 Pair 则设置一些信息
    if(iterator == this->_sourceAddressMap.end()){
        clientPairContext->_sourceAddress = sourceAddress;
        this->_sourceAddressMap.insert({ sourceAddress, pairPtr });
    }

    // 写入数据
    return pairPtr->send(payload, length);
}
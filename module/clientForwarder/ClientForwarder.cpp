//
// Created by Kris Allen on 2023/9/28.
//

#include "ClientForwarder.h"

omg::ClientForwarder::ClientForwarder(std::shared_ptr<ClientPairManager> clientPairManager) {
    this->_clientPairManager = std::move(clientPairManager);

    this->onPairClose = [this] (const PairPtr& pair){
        std::lock_guard<std::mutex> lockGuard(this->_locker);

        ClientPairContextPtr clientPairContext = pair->getContextPtr<ClientPairContext>();

        size_t sockAddrHash = -1;
        omg::utils::Socket::GenerateSockAddrHash(clientPairContext->_sourceAddressSockAddr, sockAddrHash);

        auto iterator = this->_sourceAddressMap.find(sockAddrHash);
        if(iterator == this->_sourceAddressMap.end())
            return;

        this->_sourceAddressMap.erase(iterator);
    };
}

size_t omg::ClientForwarder::onSend(const struct sockaddr* sourceAddress, const Byte *payload, size_t length) {
    std::lock_guard<std::mutex> lockGuard(this->_locker);

    int errCode = -1;
    size_t sockAddrHash = -1;
    PairPtr pair = nullptr;

    // 计算源地址哈希
    omg::utils::Socket::GenerateSockAddrHash(*reinterpret_cast<const sockaddr_u*>(sourceAddress), sockAddrHash);

    // 获取源地址对应的 Pair
    // 如果此来源地址没有对应的 Pair 则分配一个
    auto iterator = this->_sourceAddressMap.find(sockAddrHash);
    if(iterator == this->_sourceAddressMap.end()){

        // 创建 Pair
        errCode = this->_clientPairManager->createPair(pair);
        if(errCode != 0){
            LOGGER_WARN("Failed to create pair, err code:{}", errCode);
            return -1;
        }

        // 当数据回来怎么处理
        pair->onReceive = this->onReceive;

        // 注册关闭回调
        pair->addOnCloseHandler(this->onPairClose);
    }else{
        pair = iterator->second;
    }

    // 获取 Pair 上下文
    ClientPairContextPtr clientPairContext = pair->getContextPtr<ClientPairContext>();

    // 如果是新添加的 Pair 则设置一些信息
    if(iterator == this->_sourceAddressMap.end()){
        // 把地址转换成 IP:Port 格式
        char buf[SOCKADDR_STRLEN] = {0};
        std::string source = SOCKADDR_STR(sourceAddress, buf);

        clientPairContext->_sourceAddress = source;
        this->_sourceAddressMap.insert({ sockAddrHash, pair });
        LOGGER_INFO("New pair:{} <----> {}", pair->id(), "");
    }

    // 写入数据
    return pair->send(payload, length);
}
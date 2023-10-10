//
// Created by Kris on 2023/10/5.
//

#include "LibhvWsClientTunnel.h"

LibhvWsClientTunnel::LibhvWsClientTunnel(hv::EventLoopPtr eventLoop, std::string url, TunnelID tunnelID) {
    this->_tunnelID = tunnelID;
    this->_url = url;

    this->_webSocketClient = std::unique_ptr<hv::WebSocketClient>(new hv::WebSocketClient(std::move(eventLoop)));

    this->_webSocketClient->onopen = [this](){
        // TODO:
    };

    this->_webSocketClient->onmessage = [this](const std::string& payload){

        if(!this->onReceive)
            return;

        const Byte* data = (const Byte*)payload.c_str();

        this->onReceive(shared_from_this(), data, payload.size());
    };

    this->_webSocketClient->setPingInterval(1000*5);

    this->_webSocketClient->onopen = [this](){
        // TODO:
    };

    this->_reconnSetting.min_delay = 1000;
    this->_reconnSetting.max_delay = 1000 * 10;
    this->_reconnSetting.delay_policy = 2;
    this->_reconnSetting.max_retry_cnt = 20;

    this->_webSocketClient->setReconnect(&this->_reconnSetting);

    this->_webSocketClient->open(this->_url.c_str());
}

LibhvWsClientTunnel::Type LibhvWsClientTunnel::type() {
    return Type::RELIABLE;
}


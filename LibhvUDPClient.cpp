//
// Created by Kris on 2023/10/5.
//

#include "header/LibhvUDPClient.h"

Int LibhvUDPClient::init() {
//    int lastSplitIndex = this->
    this->_udpClient->createsocket(7788);
    this->_inited = true;
    return 0;
}

Int LibhvUDPClient::close() {
    this->_udpClient->stop();
    this->_udpClient->closesocket();
    return 0;
}

LibhvUDPClient::State LibhvUDPClient::state() const {
    if(!this->_inited)
        return State::INITIAL;

    return this->_udpClient->channel->isOpened() ? State::OPENED : State::CLOSED;
}

std::string LibhvUDPClient::peerAddress() const {
    return this->_udpClient->remote_host;
}

// TODO:
SizeT LibhvUDPClient::send(const Byte *, SizeT length) {

}
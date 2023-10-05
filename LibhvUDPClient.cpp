//
// Created by Kris on 2023/10/5.
//

#include "header/LibhvUDPClient.h"

Int LibhvUDPClient::init() {
    // TODO:
    this->_udpClient->createsocket(7788);
    return 0;
}

Int LibhvUDPClient::close() {
    this->_udpClient->stop();
    this->_udpClient->closesocket();
    return 0;
}

LibhvUDPClient::State LibhvUDPClient::state() const {
    return LibhvUDPClient::State::OPENED;
}

std::string LibhvUDPClient::peerAddress() const {
    return this->_udpClient->remote_host;
}

// TODO:
SizeT LibhvUDPClient::send(const Byte *, SizeT length) {

}
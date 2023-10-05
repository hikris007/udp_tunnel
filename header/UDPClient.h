//
// Created by Kris on 2023/10/5.
//

#ifndef UDP_TUNNEL_UDPCLIENT_H
#define UDP_TUNNEL_UDPCLIENT_H

#include <string>
#include <functional>
#include <memory>
#include "Byte.h"

class UDPClient {
public:
    enum State{
        OPENED,
        CLOSED
    };

    virtual State state() const = 0;
    virtual std::string peerAddress() const = 0;
    virtual SizeT send(const Byte*, SizeT length) = 0;
    virtual Int close() = 0;
    std::function<void(const Byte*,SizeT length)> onReceive = nullptr;
    std::function<void(const std::shared_ptr<UDPClient>&, State from, State state)> onStateChange = nullptr;
};

typedef std::shared_ptr<UDPClient> UDPClientPtr;

#endif //UDP_TUNNEL_UDPCLIENT_H

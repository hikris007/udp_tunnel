//
// Created by Kris on 2023/10/5.
//

#ifndef UDP_TUNNEL_UDPCLIENT_H
#define UDP_TUNNEL_UDPCLIENT_H

#include <string>
#include <functional>
#include <memory>
#include "Byte.h"

class UDPClient;
typedef std::shared_ptr<UDPClient> UDPClientPtr;

class UDPClient {
public:
    enum State{
        INITIAL,
        OPENED,
        CLOSED
    };

    // INITIAL -> OPENED -> CLOSED

    virtual State state() const = 0;
    virtual std::string peerAddress() const = 0;
    virtual SizeT send(const Byte*, SizeT length) = 0;
    virtual Int close() = 0;
    std::function<void(const Byte* payload,SizeT length)> onReceive = nullptr;
};


#endif //UDP_TUNNEL_UDPCLIENT_H

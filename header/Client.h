//
// Created by Kris Allen on 2023/9/28.
//

#ifndef UDP_TUNNEL_CLIENT_H
#define UDP_TUNNEL_CLIENT_H

#include "hv/EventLoop.h"
#include "hv/UdpServer.h"
#include "config.h"
#include "ClientForwarder.h"

class Client {
public:
    Client(Config* config);
    Int run();

private:
    void garbageCollection();

protected:
private:
    Config* _config = nullptr;
    hv::EventLoopPtr _eventLoopPtr = nullptr;
    std::unique_ptr<ClientForwarder> _clientForwarderPtr = nullptr;
    std::unique_ptr<hv::UdpServer> _udpServerPtr;
};


#endif //UDP_TUNNEL_CLIENT_H

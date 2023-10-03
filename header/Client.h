//
// Created by Kris Allen on 2023/9/28.
//

#ifndef UDP_TUNNEL_CLIENT_H
#define UDP_TUNNEL_CLIENT_H

#include "hv/EventLoop.h"
#include "hv/UdpServer.h"
#include "Config.h"
#include "ClientForwarder.h"
#include "Logger.h"

class Client {
public:
    explicit Client(Config* config);
    Int run();
    Int shutdown();

private:
    Int init();
    void garbageCollection();

protected:
private:
    std::mutex _locker;
    bool isRunning = false;

    Config* _config = nullptr;

    hv::EventLoopPtr _eventLoop = nullptr;
    hv::TimerID gcTimerID = INVALID_TIMER_ID;

    std::unique_ptr<ClientForwarder> _clientForwarder = nullptr;
    std::shared_ptr<ClientPairManager> _clientPairManager = nullptr;


    std::unique_ptr<hv::UdpServer> _udpServer = nullptr;
};


#endif //UDP_TUNNEL_CLIENT_H

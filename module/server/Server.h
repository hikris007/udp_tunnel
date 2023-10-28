//
// Created by Kris Allen on 2023/10/2.
//

#ifndef UDP_TUNNEL_SERVER_H
#define UDP_TUNNEL_SERVER_H

#include "../../header/AppContext.h"
#include "../serverPairManager/ServerPairManager.h"
#include "../listener/Listener.h"
#include "../listener/ListenerFactory.h"
#include "../logger/Logger.h"

namespace omg {

    class Server {
    public:
        explicit Server(AppContext* appContext);

        int run();
        int shutdown();

    private:
        bool _isRunning = false;
        std::mutex _runMutex;
        std::mutex _shutdownMutex;

        AppContext* _appContext = nullptr;
        hv::EventLoopPtr _eventLoop = nullptr;
        ListenerPtr _listener;
        std::shared_ptr<ServerPairManager> _serverPairManager = nullptr;
    };
}



#endif //UDP_TUNNEL_SERVER_H

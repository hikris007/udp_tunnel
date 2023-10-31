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
#include "../utils/time.hpp"

namespace omg {

    class Server {
    public:
        explicit Server(AppContext* appContext);

        int run();
        int shutdown();

    private:
        /*!
         * GC清理函数 会清理所有长时间没有使用的 Pair
         * 线程安全
         */
        void garbageCollection();

    private:
        bool _isRunning = false;
        std::mutex _runMutex;
        std::mutex _shutdownMutex;
        std::mutex _gcMutex;
        hv::TimerID gcTimerID = INVALID_TIMER_ID;

        AppContext* _appContext = nullptr;
        hv::EventLoopPtr _eventLoop = nullptr;
        ListenerPtr _listener;
        std::shared_ptr<ServerPairManager> _serverPairManager = nullptr;
    };
}



#endif //UDP_TUNNEL_SERVER_H

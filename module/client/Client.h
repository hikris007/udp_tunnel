//
// Created by Kris Allen on 2023/9/28.
//

#ifndef UDP_TUNNEL_CLIENT_H
#define UDP_TUNNEL_CLIENT_H

#include <chrono>
#include <vector>
#include "hv/EventLoop.h"
#include "hv/UdpServer.h"
#include "hv/hloop.h"
#include "../../header/AppContext.h"
#include "../clientForwarder/ClientForwarder.h"
#include "../logger/Logger.h"
#include "../utils/time.hpp"
#include "../utils/socket.hpp"
#include "../IdleHandler/IdleHandler.h"

namespace omg {
    class Client {
    public:
        explicit Client(AppContext* config);
        /*!
         * 启动 线程安全
         * @return 状态码
         * 启动后会阻塞
         */
        int run();

        /*!
         * 关闭 线程安全
         * @return 状态码
         * 0为成功
         */
        int shutdown();

    private:
        /*!
         * 初始化 初始化套接字
         * 注册发送的回调
         * 注册接受的回调
         * @return 状态码
         * 0 成功
         */
        int init();

        /*!
         * GC清理函数 会清理所有长时间没有使用的 Pair
         * 线程安全
         */
        void garbageCollection();

    protected:
    private:
        std::mutex _runMutex;
        std::mutex _shutdownMutex;
        std::mutex _gcMutex;

        bool isRunning = false;

        AppContext* _appContext = nullptr;

        hv::EventLoopPtr _eventLoop = nullptr;
        hv::TimerID _gcTimerID = INVALID_TIMER_ID;

        std::unique_ptr<ClientForwarder> _clientForwarder = nullptr;
        std::shared_ptr<ClientPairManager> _clientPairManager = nullptr;

        std::shared_ptr<hv::UdpServer> _udpServer = nullptr;
    };
}
#endif //UDP_TUNNEL_CLIENT_H

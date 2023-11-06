//
// Created by Kris on 2023/10/21.
//

#ifndef UDP_TUNNEL_LISTENERFACTORY_H
#define UDP_TUNNEL_LISTENERFACTORY_H

#include "hv/EventLoop.h"
#include "Listener.h"
#include "../../header/typedef.h"
#include "libhvWsListener/LibhvWsListener.h"

namespace omg {
    class ListenerFactory {
    public:
        // 单例 获取实例
        static ListenerFactory& getInstance();

        // 删除复制和赋值操作
        ListenerFactory(const ListenerFactory&) = delete;
        ListenerFactory& operator=(const ListenerFactory&) = delete;

        hv::EventLoopPtr getEventLoopPtr();
        void setEventLoopPtr(hv::EventLoopPtr eventLoop);
        void deleteEventLoopPtr();

        int createListener(TransportProtocol transportProtocol, const std::string& listenDescription, ListenerPtr& listener);

    private:
        ListenerFactory();

        int createLibhvWsListener( const std::string& listenDescription, ListenerPtr& listener);

    private:
        hv::EventLoopPtr _eventLoop;
    };
}

#endif //UDP_TUNNEL_LISTENERFACTORY_H

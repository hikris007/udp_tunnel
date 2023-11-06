//
// Created by Kris Allen on 2023/9/18.
//

#ifndef UDP_TUNNEL_TUNNELFACTORY_H
#define UDP_TUNNEL_TUNNELFACTORY_H

#include <queue>
#include <mutex>
#include "Tunnel.h"
#include "../../header/typedef.h"
#include "../tunnelIDPool/TunnelIDPool.h"
#include "LibhvWsTunnel/Client.h"

namespace omg {

    class TunnelFactory {
    public:
        // 单例 获取实例
        static TunnelFactory& getInstance();

        // 删除复制和赋值操作
        TunnelFactory(const TunnelFactory&) = delete;
        TunnelFactory& operator=(const TunnelFactory&) = delete;

        // 创建
        TunnelPtr createTunnel(TransportProtocol transportProtocol, const std::string& endpoint);

        hv::EventLoopPtr getEventLoopPtr();
        void setEventLoopPtr(hv::EventLoopPtr eventLoop);
        void deleteEventLoopPtr();

    private:
        // 私有构造函数，确保外部无法创建新实例
        TunnelFactory();

        TunnelPtr createWsTunnel(const std::string& endpoint);

    private:
        hv::EventLoopPtr _eventLoop;

        TunnelIDPool _tunnelIDPool;
    };

}


#endif //UDP_TUNNEL_TUNNELFACTORY_H

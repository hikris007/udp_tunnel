//
// Created by Kris Allen on 2023/9/18.
//

#ifndef UDP_TUNNEL_TUNNELFACTORY_H
#define UDP_TUNNEL_TUNNELFACTORY_H

#include <queue>
#include <mutex>
#include "Tunnel.h"
#include "../../header/typedef.h"

#include "LibhvWsClientTunnel/LibhvWsClientTunnel.h"

namespace omg {

    class TunnelFactory {
    public:
        static TunnelFactory& getInstance();
        // 删除复制和赋值操作
        TunnelFactory(const TunnelFactory&) = delete;
        TunnelFactory& operator=(const TunnelFactory&) = delete;

        Tunnel* createTunnel(TransportProtocol transportProtocol, const std::string& endpoint);

        hv::EventLoopPtr getEventLoopPtr();
        void setEventLoopPtr(hv::EventLoopPtr eventLoop);
        void deleteEventLoopPtr();

    private:
        // 私有构造函数，确保外部无法创建新实例
        TunnelFactory();

        Tunnel* createWsTunnel(const std::string& endpoint);
        void initIDPool();

    private:
        std::mutex _poolLocker;
        hv::EventLoopPtr _eventLoop;
        std::queue<TunnelID> _tunnelIDPool;
        TunnelID _lastTunnelID = INVALID_TUNNEL_ID + 1;
        size_t _poolSize = 100;
    };

}


#endif //UDP_TUNNEL_TUNNELFACTORY_H

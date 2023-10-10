//
// Created by Kris Allen on 2023/9/18.
//

#ifndef UDP_TUNNEL_APPCONTEXT_H
#define UDP_TUNNEL_APPCONTEXT_H
#include <string>
#include "typedef.h"

namespace omg {
    // 配置项
    struct ClientConfig {
        TransportProtocol transportProtocol;
        std::string listenDescription;
        std::string endpoint;
        Uint16 carryingCapacity;
    };

    // 配置项
    struct ServerConfig {
        TransportProtocol transportProtocol;
        std::string listenDescription;
        std::string endpoint;
    };

    struct AppContext {
        Int64 writeTimeout = 1000*60;
        Int64 receiveTimeout = 1000*60;
        TransportProtocol transportProtocol = TransportProtocol ::Websocket;
        RunMode runMode = RunMode::CLIENT;
        ServerConfig* serverConfig = nullptr;
        ClientConfig* clientConfig = nullptr;
    };
}
#endif //UDP_TUNNEL_APPCONTEXT_H

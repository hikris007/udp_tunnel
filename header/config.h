//
// Created by Kris Allen on 2023/9/18.
//

#ifndef UDP_TUNNEL_CONFIG_H
#define UDP_TUNNEL_CONFIG_H
#include <string>
#include "typedef.h"

typedef enum {
    Websocket,
    DTLS
} TransportProtocol;

typedef enum {
    SERVER,
    CLIENT
} RunMode;

// 配置项
struct ClientConfig {
    TransportProtocol transportProtocol;
    std::string extraData;
    Uint16 carryingCapacity;
};

// 配置项
struct ServerConfig {
    TransportProtocol transportProtocol;
    std::string extraData;
};

struct Config {
    RunMode runMode;
    ServerConfig* serverConfig;
    ClientConfig* clientConfig;
};

#endif //UDP_TUNNEL_CONFIG_H

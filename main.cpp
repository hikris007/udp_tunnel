// 系统库
#include <string>
#include <unordered_map>
#include <csignal>

// 第三方库
#include "CLI/CLI.hpp"

// 自己的文件
#include "header/Server.h"
#include "header/Client.h"
#include "header/config.h"

int main(int argc, char** argv) {
    ServerConfig serverConfig;
    ClientConfig clientConfig;

    Config config;
    config.serverConfig = &serverConfig;
    config.clientConfig = &clientConfig;

    // 参数解析
    CLI::App app("描述");

    bool serverMode = false;
    auto serverFlag = app.add_flag("-s, --server", serverMode, "服务端模式");

    bool clientMode = false;
    auto clientFlag = app.add_flag("-c, --client", clientMode, "客户端模式");

    clientFlag->excludes(serverFlag);
    serverFlag->excludes(clientFlag);

    Uint16 carryingCapacity = 4;
    app.add_option("-cc, --carrying-capacity", carryingCapacity, "多路复用数量");

    std::string transportProtocolStr = "websocket";
    app.add_option("-tp, --transport-protocol", transportProtocolStr, "协议")
        ->check(CLI::IsMember({"websocket"}));

    std::string endpoint;
    app.add_option("-ep, --endpoint", endpoint, "远程地址")->required();

    std::string listenDescription;
    app.add_option("-ld, --listen-description", listenDescription, "监听描述")->required();

    CLI11_PARSE(app, argc, argv);

    std::unordered_map<std::string, TransportProtocol> transportProtocolMap;
    transportProtocolMap.insert({ "websocket", TransportProtocol ::Websocket});
    transportProtocolMap.insert({ "dtls", TransportProtocol ::DTLS});

    TransportProtocol transportProtocol = transportProtocolMap[transportProtocolStr];

    if(serverMode){
        config.runMode = RunMode::SERVER;

        config.clientConfig->transportProtocol = transportProtocol;
        config.clientConfig->carryingCapacity = carryingCapacity;
        config.clientConfig->listenDescription = listenDescription;
        config.clientConfig->endpoint = endpoint;

        // 业务
        Server server(&config);

        auto cleanup = [](int signal){

        };

        std::signal(SIGINT, cleanup);

        server.run();
    }

    if(clientMode){
        config.runMode = RunMode::CLIENT;

        config.serverConfig->transportProtocol = transportProtocol;
        config.serverConfig->endpoint = endpoint;
        config.serverConfig->listenDescription = listenDescription;

        // 业务
        Client client(&config);

        auto cleanup = [](int signal){

        };

        std::signal(SIGINT, cleanup);

        client.run();
    }

    return 0;
}
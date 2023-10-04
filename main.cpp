// 系统库
#include <string>
#include <unordered_map>
#include <csignal>

// 第三方库
#include "CLI/CLI.hpp"

// 自己的文件
#include "header/Server.h"
#include "header/Client.h"
#include "header/AppContext.h"

ServerConfig serverConfig;
ClientConfig clientConfig;

AppContext appContext;

std::shared_ptr<Server> serverPtr = nullptr;
std::shared_ptr<Client> clientPtr = nullptr;

void handleSystemSignal(int signal);

int main(int argc, char** argv) {

    appContext.serverConfig = &serverConfig;
    appContext.clientConfig = &clientConfig;

    // 参数解析
    CLI::App app("描述");

    bool serverMode = false;
    auto serverFlag = app.add_flag("-s, --server", serverMode, "服务端模式");

    bool clientMode = false;
    auto clientFlag = app.add_flag("-c, --client", clientMode, "客户端模式");

    clientFlag->excludes(serverFlag);
    serverFlag->excludes(clientFlag);

    Uint16 carryingCapacity = 4;
    app.add_option("--cc, --carrying-capacity", carryingCapacity, "多路复用数量");

    std::string transportProtocolStr = "websocket";
    app.add_option("--tp, --transport-protocol", transportProtocolStr, "协议")
        ->check(CLI::IsMember({"websocket"}));

    std::string endpoint;
    app.add_option("--ep, --endpoint", endpoint, "远程地址")->required();

    std::string listenDescription;
    app.add_option("--ld, --listen-description", listenDescription, "监听描述")->required();

    CLI11_PARSE(app, argc, argv);

    std::unordered_map<std::string, TransportProtocol> transportProtocolMap;
    transportProtocolMap.insert({ "websocket", TransportProtocol ::Websocket});
    transportProtocolMap.insert({ "dtls", TransportProtocol ::DTLS});

    TransportProtocol transportProtocol = transportProtocolMap[transportProtocolStr];

    if(serverMode){
        appContext.runMode = RunMode::SERVER;

        appContext.serverConfig->transportProtocol = transportProtocol;
        appContext.serverConfig->endpoint = endpoint;
        appContext.serverConfig->listenDescription = listenDescription;

        // 业务
        serverPtr = std::make_shared<Server>(&appContext);
        std::signal(SIGINT, handleSystemSignal);

        serverPtr->run();
    }

    if(clientMode){
        appContext.runMode = RunMode::CLIENT;

        appContext.clientConfig->transportProtocol = transportProtocol;
        appContext.clientConfig->carryingCapacity = carryingCapacity;
        appContext.clientConfig->listenDescription = listenDescription;
        appContext.clientConfig->endpoint = endpoint;

        // 业务
        clientPtr = std::make_shared<Client>(&appContext);
        std::signal(SIGINT, handleSystemSignal);

        clientPtr->run();
    }

    getchar();

    return 0;
}

void handleSystemSignal(int signal){
    if(appContext.runMode == RunMode::SERVER){
        if(serverPtr == nullptr)
            return;

        serverPtr->shutdown();
        exit(0);
    }

    if(appContext.runMode == RunMode::CLIENT){
        if(clientPtr == nullptr)
            return;

        clientPtr->shutdown();
        exit(0);
    }
}
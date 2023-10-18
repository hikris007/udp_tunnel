// 系统库
#include <string>
#include <unordered_map>
#include <csignal>

// 第三方库
#include "CLI/CLI.hpp"

// 自己的文件
//#include "module/server/Server.h"
#include "module/client/Client.h"
#include "header/AppContext.h"

omg::ServerConfig serverConfig;
omg::ClientConfig clientConfig;

omg::AppContext appContext;

//std::shared_ptr<omg::Server> serverPtr = nullptr;
std::shared_ptr<omg::Client> clientPtr = nullptr;

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

    omg::uint16 carryingCapacity = 4;
    app.add_option("--cc, --carrying-capacity", carryingCapacity, "多路复用数量");

    std::string transportProtocolStr = "websocket";
    app.add_option("--tp, --transport-protocol", transportProtocolStr, "协议")
        ->check(CLI::IsMember({"websocket"}));

    std::string endpoint;
    app.add_option("--ep, --endpoint", endpoint, "远程地址")->required();

    std::string listenDescription;
    app.add_option("--ld, --listen-description", listenDescription, "监听描述")->required();

    CLI11_PARSE(app, argc, argv);

    std::unordered_map<std::string, omg::TransportProtocol> transportProtocolMap;
    transportProtocolMap.insert({ "websocket", omg::TransportProtocol ::Websocket});

    omg::TransportProtocol transportProtocol = transportProtocolMap[transportProtocolStr];

//    if(serverMode){
//        appContext.runMode = omg::RunMode::SERVER;
//
//        appContext.serverConfig->transportProtocol = transportProtocol;
//        appContext.serverConfig->endpoint = endpoint;
//        appContext.serverConfig->listenDescription = listenDescription;
//
//        // 业务
//        serverPtr = std::make_shared<omg::Server>(&appContext);
//        std::signal(SIGINT, handleSystemSignal);
//
//        serverPtr->run();
//    }

    if(clientMode){
        appContext.runMode = omg::RunMode::CLIENT;

        appContext.clientConfig->transportProtocol = transportProtocol;
        appContext.clientConfig->carryingCapacity = carryingCapacity;
        appContext.clientConfig->listenDescription = listenDescription;
        appContext.clientConfig->endpoint = endpoint;

        // 业务
        clientPtr = std::make_shared<omg::Client>(&appContext);
        std::signal(SIGINT, handleSystemSignal);

        clientPtr->run();
    }

    getchar();

    return 0;
}

void handleSystemSignal(int signal){
//    if(appContext.runMode == omg::RunMode::SERVER){
//        if(serverPtr == nullptr)
//            return;
//
//        serverPtr->shutdown();
//        exit(0);
//    }

    if(appContext.runMode == omg::RunMode::CLIENT){
        if(clientPtr == nullptr)
            return;

        clientPtr->shutdown();
        exit(0);
    }
}
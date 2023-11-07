//
// Created by Kris Allen on 2023/10/20.
//

#ifndef UDP_TUNNEL_LIBHVWSLISTENER_H
#define UDP_TUNNEL_LIBHVWSLISTENER_H

#include <memory>
#include "hv/WebSocketServer.h"
#include "hv/EventLoop.h"
#include "hv/hfile.h"
#include "json/json.h"
#include "../../utils/socket.hpp"
#include "../../tunnel/LibhvWsTunnel/Server.h"
#include "../Listener.h"
#include "../../tunnelIDPool/TunnelIDPool.h"

/*
 * Tunnel ----------- shared_ptr ----------- WebsocketChannel
 * WebsocketChannel ------ weak_ptr ------> Tunnel
 */

namespace omg {
    class LibhvWsServerTunnelContext {
    public:
        TunnelPtr tunnel;
    };

    class LibhvWsListenConfig {
    public:
        int fromFile(const std::string& path){
            int errCode = 0;

            std::string configJsonStr;
            HFile hFile;

            errCode = hFile.open(path.c_str(), "rb");
            if(errCode != 0)return -1;

            hFile.readall(configJsonStr);
            hFile.close();

            Json::Reader jsonReader;
            Json::Value jsonData;
            if(!jsonReader.parse(configJsonStr, jsonData))
                return -1;

            this->listen = jsonData["listen"].asString();
            this->port = jsonData["port"].asInt();
            this->https = jsonData["https"].asBool();
            this->key = jsonData["key"].asString();
            this->crt = jsonData["crt"].asString();

            if(this->listen.empty())
                this->listen = "0.0.0.0";

            if(this->port < 1)
                return -1;

            if(this->https){
                if(this->key.empty())return -1;
                if(this->crt.empty())return -1;
            }

            return 0;
        }
        int fromUri(const std::string& uri){
            int errorCode = utils::Socket::SplitIPAddress(uri, this->listen, this->port);
            if(errorCode != 0) return -1;

            return 0;
        }

        int parse(const std::string& listenDescription){
            if(strncmp(listenDescription.c_str(), "ws://", 5) == 0){
                return this->fromUri(listenDescription.substr(5));
            }else if(strncmp(listenDescription.c_str(), "file://", 7) == 0){
                return this->fromFile(listenDescription.substr(7));
            }

            return -1;
        }

    public:
        std::string listen;
        std::string key;
        std::string crt;
        int port = 0;
        bool https = false;
    };

    class LibhvWsListener : public Listener {
    public:
        explicit LibhvWsListener(hv::EventLoopPtr eventLoop);
        int start(std::string listenAddress) override;
        int stop() override;

    private:
        hv::EventLoopPtr _eventLoop;
        hv::HttpService _httpService{};
        hv::WebSocketService _webSocketService{};
        std::shared_ptr<hv::WebSocketServer> _webSocketServer;

        std::mutex _runMutex;
        std::string _listenAddress;
        bool _isRunning = false;

        TunnelIDPool _tunnelIDPool;
    };
}


#endif //UDP_TUNNEL_LIBHVWSLISTENER_H

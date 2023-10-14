//
// Created by Kris Allen on 2023/10/2.
//

#ifndef UDP_TUNNEL_SERVER_H
#define UDP_TUNNEL_SERVER_H

#include "../../header/AppContext.h"
#include "../serverPairManager/ServerPairManager.h"

namespace omg {

    class Server {
    public:
        explicit Server(AppContext* appContext);

        Int run();
        Int shutdown();

    private:
        AppContext* _appContext = nullptr;
        hv::EventLoopPtr _eventLoop = nullptr;
        std::shared_ptr<ServerPairManager> _serverPairManager = nullptr;
    };
}



#endif //UDP_TUNNEL_SERVER_H

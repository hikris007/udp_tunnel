//
// Created by Kris Allen on 2023/10/11.
//

#ifndef UDP_TUNNEL_SOCKET_H
#define UDP_TUNNEL_SOCKET_H
#include "../../header/typedef.h"

namespace omg {
    namespace utils {
        class Socket {
            Int parseSocketAddress(){
                sockaddr_u peeraddr;
                memset(&peeraddr, 0, sizeof(peeraddr));
                int ret = sockaddr_set_ipport(&peeraddr, host, port);
                if (ret != 0) {
                    return ret;
                }
                return 0;
            }
        };
    }
}

#endif //UDP_TUNNEL_SOCKET_H

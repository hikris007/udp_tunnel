//
// Created by Kris on 2023/10/5.
//

#ifndef UDP_TUNNEL_UDPCLIENTFACTORY_H
#define UDP_TUNNEL_UDPCLIENTFACTORY_H

#include "UDPClient.h"

class UDPClientFactory {
public:
    static UDPClient* createUDPClient();
};


#endif //UDP_TUNNEL_UDPCLIENTFACTORY_H

//
// Created by Kris on 2023/10/5.
//

#ifndef UDP_TUNNEL_LIBHVUDPCLIENT_H
#define UDP_TUNNEL_LIBHVUDPCLIENT_H

#include "hv/UdpClient.h"
#include "UDPClient.h"

class LibhvUDPClient : public UDPClient {
    SizeT send(const Byte *, SizeT length) override;
    Int close() override;
    State state() const override;
    std::string peerAddress() const override;

    Int init();

private:
    std::unique_ptr<hv::UdpClient> _udpClient = nullptr;
};


#endif //UDP_TUNNEL_LIBHVUDPCLIENT_H

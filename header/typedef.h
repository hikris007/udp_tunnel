//
// Created by Kris Allen on 2023/8/15.
//

#ifndef TUNNEL_TYPEDEF_H
#define TUNNEL_TYPEDEF_H

#include <cstdint>
#include <string>

namespace omg {
    typedef int Int;
    typedef std::size_t SizeT;

    typedef std::uint8_t Uint8;
    typedef std::uint16_t Uint16;
    typedef std::uint32_t Uint32;
    typedef std::uint64_t Uint64;

    typedef std::int8_t Int8;
    typedef std::int16_t Int16;
    typedef std::int32_t Int32;
    typedef std::int64_t Int64;

    typedef std::string String;

    typedef Uint8 Byte;

    typedef enum {
        Websocket,
        DTLS
    } TransportProtocol;

    typedef enum {
        SERVER,
        CLIENT
    } RunMode;
}

#endif //TUNNEL_TYPEDEF_H

//
// Created by Kris Allen on 2023/8/15.
//

#ifndef TUNNEL_TYPEDEF_H
#define TUNNEL_TYPEDEF_H

#include <cstdint>
#include <string>

namespace omg {
    typedef int Int;
    typedef std::size_t size_t;

    typedef std::uint8_t uint8_t;
    typedef std::uint16_t uint16_t;
    typedef std::uint32_t uint32_t;
    typedef std::uint64_t uint64_t;

    typedef std::int8_t int8_t;
    typedef std::int16_t int16_t;
    typedef std::int32_t int32_t;
    typedef std::int64_t int64_t;

    typedef std::string String;

    typedef uint8_t Byte;

    typedef enum {
        Websocket
    } TransportProtocol;

    typedef enum {
        SERVER,
        CLIENT
    } RunMode;
}

#endif //TUNNEL_TYPEDEF_H

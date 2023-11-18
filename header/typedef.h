//
// Created by Kris Allen on 2023/8/15.
//

#ifndef TUNNEL_TYPEDEF_H
#define TUNNEL_TYPEDEF_H

#include <cstdint>
#include <string>

namespace omg {
    typedef int int_t;
    typedef std::size_t size_t;

    typedef std::uint8_t uint8;
    typedef std::uint16_t uint16;
    typedef std::uint32_t uint32;
    typedef std::uint64_t uint64;

    typedef std::int8_t int8;
    typedef std::int16_t int16;
    typedef std::int32_t int32;
    typedef std::int64_t int64;

    typedef std::string String;

    typedef uint8_t Byte;

    typedef enum {
        Websocket
    } TransportProtocol;

    typedef enum {
        DEBUG,
        INFO,
        WARN,
        ERR
    } LoggerLevel;

    typedef enum {
        SERVER,
        CLIENT
    } RunMode;
}

#endif //TUNNEL_TYPEDEF_H

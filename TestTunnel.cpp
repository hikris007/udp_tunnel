//
// Created by Kris Allen on 2023/10/4.
//

#include "header/TestTunnel.h"

TunnelID TestTunnel::id() {
    return 10;
}

SizeT TestTunnel::send(const Byte *payload, SizeT len) {
    return 0;
}

TestTunnel::Type TestTunnel::type() {
    return TestTunnel::Type::RELIABLE;
}

Int TestTunnel::destroy() {
    return 0;
}

TestTunnel::State TestTunnel::state() {
    return TestTunnel::State::CONNECTED;
}
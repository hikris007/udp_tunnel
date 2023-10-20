#ifndef UDP_TUNNEL_HTTP_H
#define UDP_TUNNEL_HTTP_H

#include "../../header/typedef.h"

namespace omg {
    namespace utils {
        namespace http {
            namespace websocket {
                enum WebsocketFlags {
                    // opcodes
                    WS_OP_CONTINUE = 0x0,
                    WS_OP_TEXT     = 0x1,
                    WS_OP_BINARY   = 0x2,
                    WS_OP_CLOSE    = 0x8,
                    WS_OP_PING     = 0x9,
                    WS_OP_PONG     = 0xA,

                    // marks
                    WS_FINAL_FRAME = 0x10,
                    WS_HAS_MASK    = 0x20,
                };

                // TODO:
                size_t calcWebsocketFrameSize(){
                    return 0;
                }
            }
        }
    }
}

#endif //UDP_TUNNEL_HTTP_H

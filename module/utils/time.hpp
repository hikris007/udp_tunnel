#ifndef UDP_TUNNEL_UTILS_TIME_H
#define UDP_TUNNEL_UTILS_TIME_H

#include <chrono>
#include "../../header/typedef.h"

namespace omg {
    namespace utils {
        class Time {
        public:
            static uint64 getCurrentTs() {
                auto now = std::chrono::system_clock::now();

                auto epoch_time = now.time_since_epoch();
                auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(epoch_time).count();

                return millis;
            }
        };
    }
}

#endif //UDP_TUNNEL_UTILS_TIME_H

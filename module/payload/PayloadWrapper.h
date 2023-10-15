//
// Created by Kris on 2023/9/22.
//

#ifndef UDP_TUNNEL_PAYLOADWRAPPER_H
#define UDP_TUNNEL_PAYLOADWRAPPER_H

#include "../../header/typedef.h"

namespace omg {

    class PayloadWrapper {
    public:
        PayloadWrapper();

        PayloadWrapper(const Byte* data, size_t length);

        // 获取数据的指针
        const Byte *data() const;

        // 获取当前消息的长度
        size_t length() const;

    protected:
        const Byte* _dataPtr = nullptr;
        size_t _length = 0;
    };
}


#endif //UDP_TUNNEL_PAYLOADWRAPPER_H

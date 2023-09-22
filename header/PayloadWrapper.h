//
// Created by Kris on 2023/9/22.
//

#ifndef UDP_TUNNEL_PAYLOADWRAPPER_H
#define UDP_TUNNEL_PAYLOADWRAPPER_H

#include "Byte.h"

class PayloadWrapper {
public:
    PayloadWrapper();

    PayloadWrapper(const Byte* data, SizeT length);

    // 获取数据的指针
    const Byte *data() const;

    // 获取当前消息的长度
    SizeT length() const;

protected:
    const Byte* _dataPtr = nullptr;
    SizeT _length = 0;
};


#endif //UDP_TUNNEL_PAYLOADWRAPPER_H

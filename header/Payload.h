//
// Created by Kris Allen on 2023/8/15.
//

#ifndef TUNNEL_PAYLOAD_H
#define TUNNEL_PAYLOAD_H

#include <memory>
#include "Byte.h"

class Payload {
public:
    ~Payload();

    // 根据容量大小的构造函数
    explicit Payload(SizeT capacity);

    // 创建时拷贝数据
    // buffer 要拷贝的数据
    // begin  在Message中的起始位置
    // length 要拷贝的数量
    // capacity 创建的底层容器的容量
    Payload(const Byte *buffer, SizeT begin, SizeT length, SizeT capacity);

    // 创建时拷贝数据
    // buffer 要拷贝的数据
    // begin  在Message中的起始位置
    // length 要拷贝的数量
    // 此时底层容器的数量等于begin+length
    Payload(const Byte *buffer, SizeT begin, SizeT length);

    // 创建时拷贝数据
    // buffer 要拷贝的数据
    // length 要拷贝的数量
    // 此时底层容器的数量等于length
    Payload(const Byte *buffer, SizeT length);

    // copy from
    // buffer 数据
    // begin 在底层容器起始位置
    // length 拷贝的个数
    SizeT copyFrom(const Byte *buffer, SizeT begin, SizeT length);

    // 获取数据的指针
    const Byte *data();

    // 获取当前消息的长度
    SizeT length() const;

    // 获取当前容器的容量
    SizeT capacity() const;

protected:
private:
    SizeT _capacity = 0;
    SizeT _length = 0;
    Byte *_data = nullptr;
};

typedef std::shared_ptr<Payload> PayloadPtr;

#endif //TUNNEL_PAYLOAD_H

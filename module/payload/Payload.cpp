//
// Created by Kris Allen on 2023/8/15.
//

#include "Payload.h"

#include <memory>

omg::Payload::~Payload(){
    if(this->_data == nullptr)
        return;

    delete []this->_data;
    this->_data = nullptr;
}

omg::Payload::Payload(SizeT capacity) {
    this->_data = new Byte[capacity]();
    this->_dataPtr = this->_data;
    this->_length = 0;
    this->_capacity = capacity;
}

omg::Payload::Payload(const Byte* buffer, SizeT length) {
    this->_data = new Byte[length]();
    this->_dataPtr = this->_data;
    this->_length = 0;
    this->_capacity = length;

    this->copyFrom(buffer,0,length);
}

omg::Payload::Payload(const Byte *buffer, SizeT begin, SizeT length, SizeT capacity) {
    SizeT len = begin == 0 ? length : (begin+1) + length;

    if(len > capacity)
        return;

    this->_data = new Byte[capacity]();
    this->_dataPtr = this->_data;
    this->_length = 0;
    this->_capacity = capacity;

    this->copyFrom(buffer, begin, length);
}

omg::Payload::Payload(const Byte *buffer, SizeT begin, SizeT length) {
    SizeT len = begin == 0 ? length : (begin+1) + length;

    this->_data = new Byte[len]();
    this->_dataPtr = this->_data;
    this->_length = 0;
    this->_capacity = len;

    this->copyFrom(buffer, begin, length);
}

omg::SizeT omg::Payload::copyFrom(const Byte *buffer, SizeT begin, SizeT length) {
    std::lock_guard<std::mutex> lockGuard(this->_locker);

    if(this->_data == nullptr)
        return -1;

    SizeT len = begin == 0 ? length:(begin+1)+length;
    if (len > this->_capacity)
        return -1;

    memcpy(this->_data+begin,buffer,length);
    this->_length = len;

    return length;
}

omg::SizeT omg::Payload::capacity() const {
    return this->_capacity;
}
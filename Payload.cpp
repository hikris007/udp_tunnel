//
// Created by Kris Allen on 2023/8/15.
//

#include "header/Payload.h"

#include <memory>

Payload::~Payload(){
    if(this->_data == nullptr)
        return;

    delete []this->_data;
    this->_data = nullptr;
}

Payload::Payload(SizeT capacity) {
    this->_data = new Byte[capacity]();
    this->_capacity = capacity;
}

Payload::Payload(const Byte* buffer, SizeT length) {
    this->_data = new Byte[length]();
    this->_capacity = length;

    this->copyFrom(buffer,0,length);
}

Payload::Payload(const Byte *buffer, SizeT begin, SizeT length, SizeT capacity) {
    SizeT len = begin == 0 ? length : (begin+1) + length;

    if(len > capacity)
        return;

    this->_data = new Byte[capacity]();
    this->_capacity = capacity;

    this->copyFrom(buffer, begin, length);
}

Payload::Payload(const Byte *buffer, SizeT begin, SizeT length) {
    SizeT len = begin == 0 ? length : (begin+1) + length;

    this->_data = new Byte[len]();
    this->_capacity = len;

    this->copyFrom(buffer, begin, length);
}

SizeT Payload::copyFrom(const Byte *buffer, SizeT begin, SizeT length) {
    if(this->_data == nullptr)
        return -1;

    SizeT len = begin == 0 ? length:(begin+1)+length;
    if (len > this->_capacity)
        return -1;

    memcpy(this->_data+begin,buffer,length);
    this->_length = len;

    return length;
}

const Byte *Payload::data() {
    return this->_data;
}

SizeT Payload::length() const {
    return this->_length;
}

SizeT Payload::capacity() const {
    return this->_capacity;
}
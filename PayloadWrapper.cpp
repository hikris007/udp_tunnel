//
// Created by Kris on 2023/9/22.
//

#include "header/PayloadWrapper.h"

PayloadWrapper::PayloadWrapper(){

}

PayloadWrapper::PayloadWrapper(const Byte *data, SizeT length) {
    this->_dataPtr = data;
    this->_length = length;
}

SizeT PayloadWrapper::length() const {
    return this->_length;
}

const Byte *PayloadWrapper::data() const {
    return this->_dataPtr;
}
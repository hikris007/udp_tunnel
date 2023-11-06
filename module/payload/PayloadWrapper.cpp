//
// Created by Kris on 2023/9/22.
//

#include "PayloadWrapper.h"

omg::PayloadWrapper::PayloadWrapper(){

}

omg::PayloadWrapper::PayloadWrapper(const Byte *data, size_t length) {
    this->_dataPtr = data;
    this->_length = length;
}

omg::size_t omg::PayloadWrapper::length() const {
    return this->_length;
}

const omg::Byte *omg::PayloadWrapper::data() const {
    return this->_dataPtr;
}
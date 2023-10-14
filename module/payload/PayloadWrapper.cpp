//
// Created by Kris on 2023/9/22.
//

#include "PayloadWrapper.h"

omg::PayloadWrapper::PayloadWrapper(){

}

omg::PayloadWrapper::PayloadWrapper(const Byte *data, SizeT length) {
    this->_dataPtr = data;
    this->_length = length;
}

omg::SizeT omg::PayloadWrapper::length() const {
    return this->_length;
}

const omg::Byte *omg::PayloadWrapper::data() const {
    return this->_dataPtr;
}
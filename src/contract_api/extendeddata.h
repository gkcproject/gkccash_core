#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include "libdevcore/Address.h"

enum eExtendDataType {
    EXT_DATA_STRING     = 0x00,     //  0
    EXT_DATA_DOUBLE     = 0x01,     //  1
    EXT_DATA_BOOL       = 0x02,     //  2
    EXT_DATA_INT8       = 0x03,     //  3
    EXT_DATA_UINT8      = 0x04,     //  4
    EXT_DATA_INT16      = 0x05,     //  5
    EXT_DATA_UINT16     = 0x06,     //  6
    EXT_DATA_INT32      = 0x07,     //  7
    EXT_DATA_UINT32     = 0x08,     //  8
    EXT_DATA_INT64      = 0x09,     //  9
    EXT_DATA_UINT64     = 0x0a,     //  10
    EXT_DATA_INT128     = 0x0b,     //  11
    EXT_DATA_UINT128    = 0x0c,     //  12
    EXT_DATA_INT256     = 0x0d,     //  13
    EXT_DATA_UINT256    = 0x0e,     //  14
    EXT_DATA_RESERVED   = 0x0f,     //  15
};

extern bool getExtendedData(uint32_t _height, const std::string & _key, eExtendDataType & _type, std::vector<uint8_t>& _value, dev::Address const& _owner = dev::Address());


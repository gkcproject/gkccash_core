///////////////////////////////////////////////////////////
//  contractcomponent.h
//  Created on:      19-3-2018 16:40:57
//  Original author: bille
///////////////////////////////////////////////////////////
#ifndef SUPERBITCOIN_CONTRACTBASE_H
#define SUPERBITCOIN_CONTRACTBASE_H

#include <stdint.h>
#include "util.h"
#include "txmempool.h"
#include "contract/libdevcore/FixedHash.h"


struct ByteCodeExecResult
{
    uint64_t usedGas = 0;
    CAmount refundSender = 0;           //the total amount for refund to every sender of contract tx
    std::vector<CTxOut> refundOutputs;  //the CTXout for refund amount to every sender of contract tx
    std::vector<CTransaction> valueTransfers;  //after running the contract tx ,need to run the TX
};

struct CHeightTxIndexIteratorKey
{
    unsigned int height;

    size_t GetSerializeSize(int nType, int nVersion) const
    {
        //FixMe: Numerical hard coding should be avoided in case of misleading
        // here stands for sizeof(unsigned int)
        return 4;
    }

    template<typename Stream>
    void Serialize(Stream &s,int nType, int nVersion) const //FixMe: double check if this template expanding works properly
    {
        ser_writedata32be(s, height);//FixMe: double check if this template expanding works properly
    }

    template<typename Stream>
    void Unserialize(Stream &s,int nType, int nVersion)//FixMe: double check if this template expanding works properly
    {
        height = ser_readdata32be(s);//FixMe: double check if this template expanding works properly
    }

    CHeightTxIndexIteratorKey(unsigned int _height)
    {
        height = _height;
    }

    CHeightTxIndexIteratorKey()
    {
        SetNull();
    }

    void SetNull()
    {
        height = 0;
    }
};

struct CHeightTxIndexKey
{
    unsigned int height;
    dev::h160 address;

    size_t GetSerializeSize(int nType, int nVersion) const
    {
        //FixMe: Numerical hard coding should be avoided in case of misleading
        // here stands for sizeof(unsigned int + dev::h160)
        return 24;
    }

    template<typename Stream>
    void Serialize(Stream &s,int nType, int nVersion) const     //FixMe: double check if this template expanding works properly
    {
        ser_writedata32be(s, height);//FixMe: double check if this template expanding works properly
        s << address.asBytes();
    }

    template<typename Stream>
    void Unserialize(Stream &s,int nType, int nVersion) //FixMe: double check if this template expanding works properly
    {
        height = ser_readdata32be(s);//FixMe: double check if this template expanding works properly
        std::vector<unsigned char> tmp;
        s >> tmp;
        address = dev::h160(tmp);
    }

    CHeightTxIndexKey(unsigned int _height, dev::h160 _address)
    {
        height = _height;
        address = _address;
    }

    CHeightTxIndexKey()
    {
        SetNull();
    }

    void SetNull()
    {
        height = 0;
        address.clear();
    }
};

#endif //SUPERBITCOIN_CONTRACTBASE_H



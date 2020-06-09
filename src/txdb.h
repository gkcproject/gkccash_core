// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2016-2017 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_TXDB_H
#define BITCOIN_TXDB_H

#include "leveldbwrapper.h"
#include "main.h"
#include "primitives/zerocoin.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

class CCoins;
class uint256;

//! -dbcache default (MiB)
static const int64_t nDefaultDbCache = 100;
//! max. -dbcache in (MiB)
static const int64_t nMaxDbCache = sizeof(void*) > 4 ? 4096 : 1024;
//! min. -dbcache in (MiB)
static const int64_t nMinDbCache = 4;

/** CCoinsView backed by the LevelDB coin database (chainstate/) */
class CCoinsViewDB : public CCoinsView
{
protected:
    CLevelDBWrapper db;

public:
    CCoinsViewDB(size_t nCacheSize, bool fMemory = false, bool fWipe = false);

    bool GetCoins(const uint256& txid, CCoins& coins) const;
    bool HaveCoins(const uint256& txid) const;
    uint256 GetBestBlock() const;
    bool BatchWrite(CCoinsMap& mapCoins, const uint256& hashBlock);
    bool GetStats(CCoinsStats& stats) const;
};

/** Access to the block database (blocks/index/) */
class CBlockTreeDB : public CLevelDBWrapper
{
public:
    CBlockTreeDB(size_t nCacheSize, bool fMemory = false, bool fWipe = false);

private:
    CBlockTreeDB(const CBlockTreeDB&);
    void operator=(const CBlockTreeDB&);

public:
    bool WriteBlockIndex(const CDiskBlockIndex& blockindex);
    bool ReadBlockFileInfo(int nFile, CBlockFileInfo& fileinfo);
    bool WriteBlockFileInfo(int nFile, const CBlockFileInfo& fileinfo);
    bool ReadLastBlockFile(int& nFile);
    bool WriteLastBlockFile(int nFile);
    bool WriteReindexing(bool fReindex);
    bool ReadReindexing(bool& fReindex);
    bool ReadTxIndex(const uint256& txid, CDiskTxPos& pos);
    bool WriteTxIndex(const std::vector<std::pair<uint256, CDiskTxPos> >& list);
    bool WriteFlag(const std::string& name, bool fValue);
    bool ReadFlag(const std::string& name, bool& fValue);
    bool WriteInt(const std::string& name, int nValue);
    bool ReadInt(const std::string& name, int& nValue);
    bool LoadBlockIndexGuts();


    ////////////////////////////////////////////////////////////////////////////// // gkc-evm

    template<typename K>
    bool GetKey(leveldb::Iterator *piter, K &key)
    {
        leveldb::Slice slKey = piter->key();
        try
        {
            CDataStream ssKey(slKey.data(), slKey.data() + slKey.size(), SER_DISK, CLIENT_VERSION);
            ssKey >> key;
        } catch (const std::exception &)
        {
            return false;
        }
        return true;
    }

    template<typename V>
    bool GetValue(leveldb::Iterator *piter,V &value)
    {
        leveldb::Slice slValue = piter->value();
        try
        {
            CDataStream ssValue(slValue.data(), slValue.data() + slValue.size(), SER_DISK, CLIENT_VERSION);
          //  ssValue.Xor(dbwrapper_private::GetObfuscateKey(parent));
            ssValue >> value;
        } catch (const std::exception &)
        {
            return false;
        }
        return true;
    }

    bool WriteHeightIndex(const CHeightTxIndexKey &heightIndex, const std::vector<uint256>& hash);

    /**
     * Iterates through blocks by height, starting from low.
     *
     * @param low start iterating from this block height
     * @param high end iterating at this block height (ignored if <= 0)
     * @param minconf stop iterating of the block height does not have enough confirmations (ignored if <= 0)
     * @param blocksOfHashes transaction hashes in blocks iterated are collected into this vector.
     * @param addresses filter out a block unless it matches one of the addresses in this set.
     *
     * @return the height of the latest block iterated. 0 if no block is iterated.
     */
    int ReadHeightIndex(int low, int high, int minconf,
                        std::vector<std::vector<uint256>> &blocksOfHashes,
                        std::set<dev::h160> const &addresses);
    bool EraseHeightIndex(const unsigned int &height);
    bool WipeHeightIndex();

    //////////////////////////////////////////////////////////////////////////////

};

class CZerocoinDB : public CLevelDBWrapper
{
public:
    CZerocoinDB(size_t nCacheSize, bool fMemory = false, bool fWipe = false);

private:
    CZerocoinDB(const CZerocoinDB&);
    void operator=(const CZerocoinDB&);

public:
    bool WriteCoinMint(const libzerocoin::PublicCoin& pubCoin, const uint256& txHash);
    bool ReadCoinMint(const CBigNum& bnPubcoin, uint256& txHash);
    bool WriteCoinSpend(const CBigNum& bnSerial, const uint256& txHash);
    bool ReadCoinSpend(const CBigNum& bnSerial, uint256& txHash);
    bool EraseCoinMint(const CBigNum& bnPubcoin);
    bool EraseCoinSpend(const CBigNum& bnSerial);
    bool WriteAccumulatorValue(const uint32_t& nChecksum, const CBigNum& bnValue);
    bool ReadAccumulatorValue(const uint32_t& nChecksum, CBigNum& bnValue);
    bool EraseAccumulatorValue(const uint32_t& nChecksum);
};

class CAddressDB : public CLevelDBWrapper
{
    CAddressDB(const CAddressDB&);
    void operator=(const CAddressDB&);

public:
    CAddressDB(size_t nCacheSize, bool fMemory, bool fWipe);

    bool AddTx(const std::vector<CTransaction>& vtx, const std::vector<std::pair<uint256, CDiskTxPos> >& vpos);
    bool GetTxs(std::vector<CDiskTxPos>& Txs, const CScriptID& Address);
    bool ReadNextIn(const COutPoint& Out, uint256& Hash, unsigned int &n);

    bool WriteReindexing(bool fReindex);
    bool ReadReindexing(bool &fReindex);

    bool WriteEnable(bool fValue);
    bool ReadEnable(bool &fValue);
};

CTxOut getPrevOut(const CTxIn& In);
void getNextIn(const COutPoint& Out, uint256& Hash, unsigned int& n);
// Return transaction in tx, and if it was found inside a block, its header is placed in block
bool ReadTransaction(const CDiskTxPos &postx, CTransaction &txOut, CBlockHeader &block);


#endif // BITCOIN_TXDB_H

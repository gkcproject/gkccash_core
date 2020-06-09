// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2016-2017 The PIVX developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "txdb.h"

#include "main.h"
#include "pow.h"
#include "uint256.h"
#include "accumulators.h"

#include <stdint.h>

#include <boost/thread.hpp>
#include "crp/CoinReleasePlan.h"

////////////////////////////////////////// // gkc-vm
static const char DB_HEIGHTINDEX = 'h';
//////////////////////////////////////////

using namespace std;
using namespace libzerocoin;

void static BatchWriteCoins(CLevelDBBatch& batch, const uint256& hash, const CCoins& coins)
{
    if (coins.IsPruned())
        batch.Erase(make_pair('c', hash));
    else
        batch.Write(make_pair('c', hash), coins);
}

void static BatchWriteHashBestChain(CLevelDBBatch& batch, const uint256& hash)
{
    batch.Write('B', hash);
}

CCoinsViewDB::CCoinsViewDB(size_t nCacheSize, bool fMemory, bool fWipe) : db(GetDataDir() / "chainstate", nCacheSize, fMemory, fWipe)
{
}

bool CCoinsViewDB::GetCoins(const uint256& txid, CCoins& coins) const
{
    return db.Read(make_pair('c', txid), coins);
}

bool CCoinsViewDB::HaveCoins(const uint256& txid) const
{
    return db.Exists(make_pair('c', txid));
}

uint256 CCoinsViewDB::GetBestBlock() const
{
    uint256 hashBestChain;
    if (!db.Read('B', hashBestChain))
        return uint256(0);
    return hashBestChain;
}

bool CCoinsViewDB::BatchWrite(CCoinsMap& mapCoins, const uint256& hashBlock)
{
    CLevelDBBatch batch;
    size_t count = 0;
    size_t changed = 0;
    for (CCoinsMap::iterator it = mapCoins.begin(); it != mapCoins.end();) {
        if (it->second.flags & CCoinsCacheEntry::DIRTY) {
            BatchWriteCoins(batch, it->first, it->second.coins);
            changed++;
        }
        count++;
        CCoinsMap::iterator itOld = it++;
        mapCoins.erase(itOld);
    }
    if (hashBlock != uint256(0))
        BatchWriteHashBestChain(batch, hashBlock);

    LogPrint("coindb", "Committing %u changed transactions (out of %u) to coin database...\n", (unsigned int)changed, (unsigned int)count);
    return db.WriteBatch(batch);
}

CBlockTreeDB::CBlockTreeDB(size_t nCacheSize, bool fMemory, bool fWipe) : CLevelDBWrapper(GetDataDir() / "blocks" / "index", nCacheSize, fMemory, fWipe)
{
}

bool CBlockTreeDB::WriteBlockIndex(const CDiskBlockIndex& blockindex)
{
	//LogPrintf("DPOS CBlockTreeDB::WriteBlockIndex | %s\n",blockindex.ToString());
    return Write(make_pair('b', blockindex.GetBlockHash()), blockindex);
}

bool CBlockTreeDB::WriteBlockFileInfo(int nFile, const CBlockFileInfo& info)
{
    return Write(make_pair('f', nFile), info);
}

bool CBlockTreeDB::ReadBlockFileInfo(int nFile, CBlockFileInfo& info)
{
    return Read(make_pair('f', nFile), info);
}

bool CBlockTreeDB::WriteLastBlockFile(int nFile)
{
    return Write('l', nFile);
}

bool CBlockTreeDB::WriteReindexing(bool fReindexing)
{
    if (fReindexing)
        return Write('R', '1');
    else
        return Erase('R');
}

bool CBlockTreeDB::ReadReindexing(bool& fReindexing)
{
    fReindexing = Exists('R');
    return true;
}

bool CBlockTreeDB::ReadLastBlockFile(int& nFile)
{
    return Read('l', nFile);
}

bool CCoinsViewDB::GetStats(CCoinsStats& stats) const
{
    /* It seems that there are no "const iterators" for LevelDB.  Since we
       only need read operations on it, use a const-cast to get around
       that restriction.  */
    boost::scoped_ptr<leveldb::Iterator> pcursor(const_cast<CLevelDBWrapper*>(&db)->NewIterator());
    pcursor->SeekToFirst();

    CHashWriter ss(SER_GETHASH, PROTOCOL_VERSION);
    stats.hashBlock = GetBestBlock();
    ss << stats.hashBlock;
    CAmount nTotalAmount = 0;
    while (pcursor->Valid()) {
        boost::this_thread::interruption_point();
        try {
            leveldb::Slice slKey = pcursor->key();
            CDataStream ssKey(slKey.data(), slKey.data() + slKey.size(), SER_DISK, CLIENT_VERSION);
            char chType;
            ssKey >> chType;
            if (chType == 'c') {
                leveldb::Slice slValue = pcursor->value();
                CDataStream ssValue(slValue.data(), slValue.data() + slValue.size(), SER_DISK, CLIENT_VERSION);
                CCoins coins;
                ssValue >> coins;
                uint256 txhash;
                ssKey >> txhash;
                ss << txhash;
                ss << VARINT(coins.nVersion);
                ss << (coins.fCoinBase ? 'c' : 'n');
                ss << VARINT(coins.nHeight);
                stats.nTransactions++;
                for (unsigned int i = 0; i < coins.vout.size(); i++) {
                    const CTxOut& out = coins.vout[i];
                    if (!out.IsNull()) {
                        stats.nTransactionOutputs++;
                        ss << VARINT(i + 1);
                        ss << out;
                        nTotalAmount += out.nValue;
                    }
                }
                stats.nSerializedSize += 32 + slValue.size();
                ss << VARINT(0);
            }
            pcursor->Next();
        } catch (std::exception& e) {
            return error("%s : Deserialize or I/O error - %s", __func__, e.what());
        }
    }
    stats.nHeight = mapBlockIndex.find(GetBestBlock())->second->nHeight;
    stats.hashSerialized = ss.GetHash();
    stats.nTotalAmount = nTotalAmount;
    return true;
}

bool CBlockTreeDB::ReadTxIndex(const uint256& txid, CDiskTxPos& pos)
{
    return Read(make_pair('t', txid), pos);
}

bool CBlockTreeDB::WriteTxIndex(const std::vector<std::pair<uint256, CDiskTxPos> >& vect)
{
    CLevelDBBatch batch;
    for (std::vector<std::pair<uint256, CDiskTxPos> >::const_iterator it = vect.begin(); it != vect.end(); it++)
        batch.Write(make_pair('t', it->first), it->second);
    return WriteBatch(batch);
}

bool CBlockTreeDB::WriteFlag(const std::string& name, bool fValue)
{
    return Write(std::make_pair('F', name), fValue ? '1' : '0');
}

bool CBlockTreeDB::ReadFlag(const std::string& name, bool& fValue)
{
    char ch;
    if (!Read(std::make_pair('F', name), ch))
        return false;
    fValue = ch == '1';
    return true;
}

bool CBlockTreeDB::WriteInt(const std::string& name, int nValue)
{
    return Write(std::make_pair('I', name), nValue);
}

bool CBlockTreeDB::ReadInt(const std::string& name, int& nValue)
{
    return Read(std::make_pair('I', name), nValue);
}

/////////////////////////////////////////////////////// // gkc-vm
bool CBlockTreeDB::WriteHeightIndex(const CHeightTxIndexKey &heightIndex, const std::vector<uint256>& hash) {
    CLevelDBBatch batch;
    batch.Write(std::make_pair(DB_HEIGHTINDEX, heightIndex), hash);
    return WriteBatch(batch);
}



int CBlockTreeDB::ReadHeightIndex(int low, int high, int minconf,
                                  std::vector<std::vector<uint256>> &blocksOfHashes,
                                  std::set<dev::h160> const &addresses) {

    if ((high < low && high > -1) || (high == 0 && low == 0) || (high < -1 || low < 0)) {
        return -1;
    }

    boost::scoped_ptr<leveldb::Iterator> pcursor(NewIterator());

    CDataStream ssKeySet(SER_DISK, CLIENT_VERSION);

    ssKeySet.reserve(ssKeySet.GetSerializeSize(make_pair(DB_HEIGHTINDEX, CHeightTxIndexIteratorKey(low))));
    ssKeySet << make_pair(DB_HEIGHTINDEX, CHeightTxIndexIteratorKey(low));

    leveldb::Slice slKey(&ssKeySet[0], ssKeySet.size());
    pcursor->Seek(slKey);

    int curheight = 0;

    for (size_t count = 0; pcursor->Valid(); pcursor->Next()) {

        std::pair<char, CHeightTxIndexKey> key;
        if (!GetKey(pcursor.get(),key) || key.first != DB_HEIGHTINDEX) {
            LogPrintf("ReadHeightIndex failed, check whether WriteHeightIndex is ok!");
            break;
        }

        int nextHeight = key.second.height;

        if (high > -1 && nextHeight > high) {
            break;
        }

        if (minconf > 0) {

            int conf = chainActive.Height() - nextHeight;
            if (conf < minconf) {
                break;
            }
        }

        curheight = nextHeight;

        auto address = key.second.address;
        if (!addresses.empty() && addresses.find(address) == addresses.end()) {
            continue;
        }

        std::vector<uint256> hashesTx;

        if (!GetValue(pcursor.get(),hashesTx)) {
            break;
        }

        count += hashesTx.size();

        blocksOfHashes.push_back(hashesTx);
    }

    return curheight;
}

bool CBlockTreeDB::EraseHeightIndex(const unsigned int &height) {


    boost::scoped_ptr<leveldb::Iterator> pcursor(NewIterator());
    CLevelDBBatch batch;

    CDataStream ssKeySet(SER_DISK, CLIENT_VERSION);

    ssKeySet.reserve(ssKeySet.GetSerializeSize(make_pair(DB_HEIGHTINDEX, CHeightTxIndexIteratorKey(height))));
    ssKeySet << make_pair(DB_HEIGHTINDEX, CHeightTxIndexIteratorKey(height));

    leveldb::Slice slKey(&ssKeySet[0], ssKeySet.size());

    pcursor->Seek(slKey);


    while (pcursor->Valid()) {
        boost::this_thread::interruption_point();
        std::pair<char, CHeightTxIndexKey> key;
        if (GetKey(pcursor.get(),key) && key.first == DB_HEIGHTINDEX && key.second.height == height) {
            batch.Erase(key);
            pcursor->Next();
        } else {
            break;
        }
    }

    return WriteBatch(batch);
}

bool CBlockTreeDB::WipeHeightIndex() {

    boost::scoped_ptr<leveldb::Iterator> pcursor(NewIterator());
    CLevelDBBatch batch;

    CDataStream ssKeySet(SER_DISK, CLIENT_VERSION);

    ssKeySet.reserve(ssKeySet.GetSerializeSize(DB_HEIGHTINDEX));

    ssKeySet << DB_HEIGHTINDEX;

    leveldb::Slice slKey(&ssKeySet[0], ssKeySet.size());
    pcursor->Seek(slKey);

    while (pcursor->Valid()) {
        boost::this_thread::interruption_point();
        std::pair<char, CHeightTxIndexKey> key;
        if (GetKey(pcursor.get(),key) && key.first == DB_HEIGHTINDEX) {
            batch.Erase(key);
            pcursor->Next();
        } else {
            break;
        }
    }

    return WriteBatch(batch);
}

///////////////////////////////////////////////////////


bool CBlockTreeDB::LoadBlockIndexGuts()
{
    boost::scoped_ptr<leveldb::Iterator> pcursor(NewIterator());

    CDataStream ssKeySet(SER_DISK, CLIENT_VERSION);
    ssKeySet << make_pair('b', uint256(0));
    pcursor->Seek(ssKeySet.str());

    // Load mapBlockIndex
    uint256 nPreviousCheckpoint;
    while (pcursor->Valid()) {
        boost::this_thread::interruption_point();
        try {
            leveldb::Slice slKey = pcursor->key();
            CDataStream ssKey(slKey.data(), slKey.data() + slKey.size(), SER_DISK, CLIENT_VERSION);
            char chType;
            ssKey >> chType;
            if (chType == 'b') {
                leveldb::Slice slValue = pcursor->value();
                CDataStream ssValue(slValue.data(), slValue.data() + slValue.size(), SER_DISK, CLIENT_VERSION);
                CDiskBlockIndex diskindex;
                ssValue >> diskindex;

                // Construct block index object
                CBlockIndex* pindexNew = InsertBlockIndex(diskindex.GetBlockHash());
                pindexNew->pprev = InsertBlockIndex(diskindex.hashPrev);
                pindexNew->pnext = InsertBlockIndex(diskindex.hashNext);
                pindexNew->nHeight = diskindex.nHeight;
                pindexNew->nFile = diskindex.nFile;
                pindexNew->nDataPos = diskindex.nDataPos;
                pindexNew->nUndoPos = diskindex.nUndoPos;
                pindexNew->nVersion = diskindex.nVersion;
                pindexNew->hashMerkleRoot = diskindex.hashMerkleRoot;
                pindexNew->nTime = diskindex.nTime;
                pindexNew->nBits = diskindex.nBits;
                pindexNew->nNonce = diskindex.nNonce;
                pindexNew->nStatus = diskindex.nStatus;
                pindexNew->nTx = diskindex.nTx;

                //zerocoin
                pindexNew->nAccumulatorCheckpoint = diskindex.nAccumulatorCheckpoint;
                pindexNew->mapZerocoinSupply = diskindex.mapZerocoinSupply;
                pindexNew->vMintDenominationsInBlock = diskindex.vMintDenominationsInBlock;

                //Proof Of Stake
                pindexNew->nMint = diskindex.nMint;
                pindexNew->nMoneySupply = diskindex.nMoneySupply;
                pindexNew->nFlags = diskindex.nFlags;
                pindexNew->nStakeModifier = diskindex.nStakeModifier;
                pindexNew->prevoutStake = diskindex.prevoutStake;
                pindexNew->nStakeTime = diskindex.nStakeTime;
                pindexNew->hashProofOfStake = diskindex.hashProofOfStake;

                if (crp::IsPowHeight(pindexNew->nHeight)) {
					//LogPrintf("DPOS CBlockTreeDB::LoadBlockIndexGuts | %s\n",pindexNew->ToString());
                    if (!CheckProofOfWork(pindexNew->GetBlockPoWHash(), pindexNew->nBits))
                        return error("LoadBlockIndex() : CheckProofOfWork failed: %s", pindexNew->ToString());
                }
                // ppcoin: build setStakeSeen
                if (pindexNew->IsProofOfStake())
                    setStakeSeen.insert(make_pair(pindexNew->prevoutStake, pindexNew->nStakeTime));

                //populate accumulator checksum map in memory
                if(pindexNew->nAccumulatorCheckpoint != 0 && pindexNew->nAccumulatorCheckpoint != nPreviousCheckpoint) {
                    //Don't load any invalid checkpoints
                    if (!InvalidCheckpointRange(pindexNew->nHeight))
                        LoadAccumulatorValuesFromDB(pindexNew->nAccumulatorCheckpoint);

                    nPreviousCheckpoint = pindexNew->nAccumulatorCheckpoint;
                }

                pcursor->Next();
            } else {
                break; // if shutdown requested or finished loading block index
            }
        } catch (std::exception& e) {
            return error("%s : Deserialize or I/O error - %s", __func__, e.what());
        }
    }

    return true;
}

CZerocoinDB::CZerocoinDB(size_t nCacheSize, bool fMemory, bool fWipe) : CLevelDBWrapper(GetDataDir() / "zerocoin", nCacheSize, fMemory, fWipe)
{
}

bool CZerocoinDB::WriteCoinMint(const PublicCoin& pubCoin, const uint256& hashTx)
{
    CBigNum bnValue = pubCoin.getValue();
    CDataStream ss(SER_GETHASH, 0);
    ss << pubCoin.getValue();
    uint256 hash = Hash(ss.begin(), ss.end());

    return Write(make_pair('m', hash), hashTx, true);
}

bool CZerocoinDB::ReadCoinMint(const CBigNum& bnPubcoin, uint256& hashTx)
{
    CDataStream ss(SER_GETHASH, 0);
    ss << bnPubcoin;
    uint256 hash = Hash(ss.begin(), ss.end());

    return Read(make_pair('m', hash), hashTx);
}

bool CZerocoinDB::EraseCoinMint(const CBigNum& bnPubcoin)
{
    CDataStream ss(SER_GETHASH, 0);
    ss << bnPubcoin;
    uint256 hash = Hash(ss.begin(), ss.end());

    return Erase(make_pair('m', hash));
}

bool CZerocoinDB::WriteCoinSpend(const CBigNum& bnSerial, const uint256& txHash)
{
    CDataStream ss(SER_GETHASH, 0);
    ss << bnSerial;
    uint256 hash = Hash(ss.begin(), ss.end());

    return Write(make_pair('s', hash), txHash, true);
}

bool CZerocoinDB::ReadCoinSpend(const CBigNum& bnSerial, uint256& txHash)
{
    CDataStream ss(SER_GETHASH, 0);
    ss << bnSerial;
    uint256 hash = Hash(ss.begin(), ss.end());

    return Read(make_pair('s', hash), txHash);
}

bool CZerocoinDB::EraseCoinSpend(const CBigNum& bnSerial)
{
    CDataStream ss(SER_GETHASH, 0);
    ss << bnSerial;
    uint256 hash = Hash(ss.begin(), ss.end());

    return Erase(make_pair('s', hash));
}

bool CZerocoinDB::WriteAccumulatorValue(const uint32_t& nChecksum, const CBigNum& bnValue)
{
    LogPrint("zero","%s : checksum:%d val:%s\n", __func__, nChecksum, bnValue.GetHex());
    return Write(make_pair('a', nChecksum), bnValue);
}

bool CZerocoinDB::ReadAccumulatorValue(const uint32_t& nChecksum, CBigNum& bnValue)
{
    return Read(make_pair('a', nChecksum), bnValue);
}

bool CZerocoinDB::EraseAccumulatorValue(const uint32_t& nChecksum)
{
    LogPrint("zero", "%s : checksum:%d\n", __func__, nChecksum);
    return Erase(make_pair('a', nChecksum));
}

CAddressDB::CAddressDB(size_t nCacheSize, bool fMemory, bool fWipe) : CLevelDBWrapper(GetDataDir() / "blocks" / "addresses", nCacheSize, fMemory, fWipe)
{
}

bool CAddressDB::AddTx(const std::vector<CTransaction>& vtx, const std::vector<std::pair<uint256, CDiskTxPos> >& vpos)
{
    for (unsigned int i = 0; i < vtx.size(); i++)
    {
        const CDiskTxPos& pos = vpos[i].second;
        uint256 TxHash = vtx[i].GetHash();

        std::vector<CScriptID> Inputs;
        for (unsigned int j = 0; j < vtx[i].vin.size(); j++)
        {
            const CTxIn& in = vtx[i].vin[j];
            CScript script = getPrevOut(in).scriptPubKey;
            if (script.empty())
                continue;
            CKeyID KeyID;
            CTxDestination address;
            if (ExtractDestination(script, address)) {
                if (typeid(CKeyID) == address.type()) {
                    KeyID = boost:: get<CKeyID>(address);
                }
            }
            CScriptID scid(KeyID);

            if (CKeyID() == KeyID)
                scid = CScriptID(script);

            // ignore inputs from the same address
            if (std::find(Inputs.begin(), Inputs.end(), scid) != Inputs.end())
                continue;
            Inputs.push_back(scid);

            std::vector<CDiskTxPos> Txs;
            Read(scid, Txs);
            Txs.push_back(pos);
            if (!Write(scid, Txs))
                return false;

            // store 'redeemed in' information for each tx output
            std::vector<std::pair<uint256, unsigned int> > Ins;
            Read(in.prevout.hash, Ins);
            if (in.prevout.n >= Ins.size())
                Ins.resize(in.prevout.n + 1);
            Ins[in.prevout.n] = std::pair<uint256, unsigned int>(TxHash, j);
            Write(in.prevout.hash, Ins);
        }
        BOOST_FOREACH (const CTxOut& out, vtx[i].vout)
        {
            CKeyID KeyID;
            CTxDestination OutDest;
            if (ExtractDestination(out.scriptPubKey, OutDest)) {
                if (typeid(CKeyID) == OutDest.type()) {
                    KeyID = boost:: get<CKeyID>(OutDest);
                }
            }
            CScriptID scid(KeyID);

            if (CKeyID() == KeyID)
                scid = CScriptID(out.scriptPubKey);
            
            if (std::find(Inputs.begin(), Inputs.end(), scid) != Inputs.end())
                continue;
            
            std::vector<CDiskTxPos> Txs;
            Read(scid, Txs);
            Txs.push_back(pos);
            if (!Write(scid, Txs))
                return false;
        }
    }
    return true;
}

bool CAddressDB::GetTxs(std::vector<CDiskTxPos>& Txs, const CScriptID &Address)
{
    return Read(Address, Txs);
}

bool CAddressDB::ReadNextIn(const COutPoint &Out, uint256& Hash, unsigned int& n)
{
    std::vector<std::pair<uint256, unsigned int> > Ins;
    if (!Read(Out.hash, Ins) || Out.n >= Ins.size())
        return false;
    Hash = Ins[Out.n].first;
    n = Ins[Out.n].second;
    return true;
}

bool CAddressDB::WriteReindexing(bool fReindexing) {
    if (fReindexing)
        return Write('R', '1');
    else
        return Erase('R');
}

bool CAddressDB::ReadReindexing(bool &fReindexing) {
    fReindexing = Exists('R');
    return true;
}

bool CAddressDB::WriteEnable( bool fValue) {
    return Write(std::string("Faddrindex"), fValue ? '1' : '0');
}

bool CAddressDB::ReadEnable( bool &fValue) {
    char ch;
    if (!Read(std::string("Faddrindex"), ch))
        return false;
    fValue = ch == '1';
    return true;
}

CTxOut getPrevOut(const CTxIn& In)
{
    CTransaction tx;
    uint256 hashBlock = 0;
    if (GetTransaction(In.prevout.hash, tx, hashBlock, true))
        return tx.vout[In.prevout.n];
    return CTxOut();
}
void getNextIn(const COutPoint &Out, uint256& Hash, unsigned int& n)
{
    Hash = 0;
    n = 0;
    if (paddressmap)
        paddressmap->ReadNextIn(Out, Hash, n);
}

// Return transaction in tx, and if it was found inside a block, its header is placed in block
bool ReadTransaction(const CDiskTxPos &postx, CTransaction &txOut, CBlockHeader &block)
{
    CAutoFile file(OpenBlockFile(postx, true), SER_DISK, CLIENT_VERSION);
    try
    {
        file >> block;
        fseek(file.Get(), postx.nTxOffset, SEEK_CUR);
        file >> txOut;
    }
    catch (std::exception &e)
    {
        return error("%s() : deserialize or I/O error", __PRETTY_FUNCTION__);
    }
    return true;
}




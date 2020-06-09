// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2013 The Bitcoin developers
// Copyright (c) 2016-2017 The PIVX developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_WALLETDB_H
#define BITCOIN_WALLETDB_H

#include "amount.h"
#include "db.h"
#include "key.h"
#include "keystore.h"
#include "primitives/zerocoin.h"
#include "libzerocoin/Accumulator.h"
#include "libzerocoin/Denominations.h"
#include "wallet/mnemoniccontainer.h"

#include <list>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/split.hpp>

class CAccount;
class CAccountingEntry;
struct CBlockLocator;
class CKeyPool;
class CMasterKey;
class CScript;
class CWallet;
class CWalletTx;
class CZerocoinMint;
class CZerocoinSpend;
class uint160;
class uint256;
class CTokenInfo;
class CTokenTx;

static const uint32_t ORIGINAL_KEYPATH_SIZE = 0x4; // m/0'/0'/<n> is the original keypath
static const uint32_t BIP44_KEYPATH_SIZE = 0x6;    // m/44'/<1/136>'/0'/<c>/<n> is the BIP44 keypath


/** Error statuses for the wallet database */
enum DBErrors {
    DB_LOAD_OK,
    DB_CORRUPT,
    DB_NONCRITICAL_ERROR,
    DB_TOO_NEW,
    DB_LOAD_FAIL,
    DB_NEED_REWRITE
};

// {value, isHardened}
typedef pair<uint32_t,bool> Component;

/* simple HD chain data model */
class CHDChain
{
public:
    uint32_t nExternalChainCounter; // VERSION_BASIC
    vector<uint32_t> nExternalChainCounters; // VERSION_WITH_BIP44: vector index corresponds to account value
    CKeyID masterKeyID; //!< master key hash160

    static const int VERSION_BASIC = 1;
    static const int VERSION_WITH_BIP44 = 10;
    static const int VERSION_WITH_BIP39 = 11;
    static const int CURRENT_VERSION = VERSION_WITH_BIP39;
    static const int N_CHANGES = 5; // standard = 0/1, mint = 2, elysium = 3, elysiumv1 = 4
    int nVersion;

    CHDChain() { SetNull(); }
    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {

        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(nExternalChainCounter);
        READWRITE(masterKeyID);
        if (this->nVersion >= VERSION_WITH_BIP44) {
            READWRITE(nExternalChainCounters);
            nExternalChainCounters.resize(N_CHANGES);
        }
    }

    void SetNull()
    {
        nVersion = CHDChain::CURRENT_VERSION;
        masterKeyID.SetNull();
        nExternalChainCounter = 0;
        for(int index=0;index<N_CHANGES;index++){
            nExternalChainCounters.push_back(0);
        }
    }
};

class CKeyMetadata
{
public:
    static const int VERSION_BASIC=1;
    static const int VERSION_WITH_HDDATA=10;
    static const int CURRENT_VERSION=VERSION_WITH_HDDATA;
    int nVersion;
    int64_t nCreateTime; // 0 means unknown
    std::string hdKeypath; //optional HD/bip32 keypath
    Component nChange; // HD/bip32 keypath change counter
    Component nChild; // HD/bip32 keypath child counter
    CKeyID hdMasterKeyID; //id of the HD masterkey used to derive this key

    CKeyMetadata()
    {
        SetNull();
    }
    CKeyMetadata(int64_t nCreateTime_)
    {
        nVersion = CKeyMetadata::CURRENT_VERSION;
        nCreateTime = nCreateTime_;
    }

    bool ParseComponents(){
        std::vector<std::string> nComponents;
        if(hdKeypath.empty())
            return false;
        if(hdKeypath=="m")
            return true;

        boost::split(nComponents, hdKeypath, boost::is_any_of("/"), boost::token_compress_on);
        if(nComponents.size()!=ORIGINAL_KEYPATH_SIZE &&
           nComponents.size()!=BIP44_KEYPATH_SIZE)
            return false;

        std::string nChangeStr = nComponents[nComponents.size()-2];
        std::string nChildStr  = nComponents[nComponents.size()-1];

        nChange.second = (nChangeStr.find("'") != string::npos);
        boost::erase_all(nChangeStr, "'");
        nChange.first = boost::lexical_cast<uint32_t>(nChangeStr);

        nChild.second = (nChildStr.find("'") != string::npos);
        boost::erase_all(nChildStr, "'");
        nChild.first = boost::lexical_cast<uint32_t>(nChildStr);

        return true;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(nCreateTime);
        if (this->nVersion >= VERSION_WITH_HDDATA)
        {
            READWRITE(hdKeypath);
            READWRITE(hdMasterKeyID);
        }
    }

    void SetNull()
    {
        nVersion = CKeyMetadata::CURRENT_VERSION;
        nCreateTime = 0;
        hdKeypath.clear();
        nChild = Component(0, false);
        nChange = Component(0, false);
        hdMasterKeyID.SetNull();
    }
};

/** Access to the wallet database (wallet.dat) */
class CWalletDB : public CDB
{
public:
    CWalletDB(const std::string& strFilename, const char* pszMode = "r+") : CDB(strFilename, pszMode)
    {
    }

    bool WriteName(const std::string& strAddress, const std::string& strName);
    bool EraseName(const std::string& strAddress);

    bool WriteSeeds(const string& strAddress, const std::string& seeds);
    bool EraseSeeds(const std::string& strAddress);
    bool WriteChainType(const string& strAddress, int chainType);
    bool EraseChainType(const string& strAddress);

    bool WritePurpose(const std::string& strAddress, const std::string& purpose);
    bool ErasePurpose(const std::string& strAddress);

    bool WriteTx(uint256 hash, const CWalletTx& wtx);
    bool EraseTx(uint256 hash);


    bool WriteToken(const CTokenInfo& wtoken);
    bool EraseToken(uint256 hash);

    bool WriteTokenTx(const CTokenTx& wTokenTx);
    bool EraseTokenTx(uint256 hash);

    bool WriteKey(const CPubKey& vchPubKey, const CPrivKey& vchPrivKey, const CKeyMetadata& keyMeta);
    bool WriteCryptedKey(const CPubKey& vchPubKey, const std::vector<unsigned char>& vchCryptedSecret, const CKeyMetadata& keyMeta);
    bool WriteMasterKey(unsigned int nID, const CMasterKey& kMasterKey);

    bool WriteCScript(const uint160& hash, const CScript& redeemScript);

    bool WriteWatchOnly(const CScript& script);
    bool EraseWatchOnly(const CScript& script);

    bool WriteMultiSig(const CScript& script);
    bool EraseMultiSig(const CScript& script);

    bool WriteBestBlock(const CBlockLocator& locator);
    bool ReadBestBlock(CBlockLocator& locator);

    bool WriteOrderPosNext(int64_t nOrderPosNext);

    // presstab
    bool WriteStakeSplitThreshold(uint64_t nStakeSplitThreshold);
    bool WriteMultiSend(std::vector<std::pair<std::string, int> > vMultiSend);
    bool EraseMultiSend(std::vector<std::pair<std::string, int> > vMultiSend);
    bool WriteMSettings(bool fMultiSendStake, bool fMultiSendMasternode, int nLastMultiSendHeight);
    bool WriteMSDisabledAddresses(std::vector<std::string> vDisabledAddresses);
    bool EraseMSDisabledAddresses(std::vector<std::string> vDisabledAddresses);
    bool WriteAutoCombineSettings(bool fEnable, CAmount nCombineThreshold);

    bool WriteDefaultKey(const CPubKey& vchPubKey);

    bool ReadPool(int64_t nPool, CKeyPool& keypool);
    bool WritePool(int64_t nPool, const CKeyPool& keypool);
    bool ErasePool(int64_t nPool);

    bool WriteMinVersion(int nVersion);

    bool ReadAccount(const std::string& strAccount, CAccount& account);
    bool WriteAccount(const std::string& strAccount, const CAccount& account);

    /// Write destination data key,value tuple to database
    bool WriteDestData(const std::string& address, const std::string& key, const std::string& value);
    /// Erase destination data tuple from wallet database
    bool EraseDestData(const std::string& address, const std::string& key);

    /// Write contract data key,value tuple to database
    bool WriteContractData(const std::string &address, const std::string &key, const std::string &value);
    /// Erase contract data tuple from wallet database
    bool EraseContractData(const std::string &address, const std::string &key);


    bool WriteAccountingEntry(const CAccountingEntry& acentry);
    CAmount GetAccountCreditDebit(const std::string& strAccount);
    void ListAccountCreditDebit(const std::string& strAccount, std::list<CAccountingEntry>& acentries);

    DBErrors ReorderTransactions(CWallet* pwallet);
    DBErrors LoadWallet(CWallet* pwallet);
    DBErrors FindWalletTx(CWallet* pwallet, std::vector<uint256>& vTxHash, std::vector<CWalletTx>& vWtx);
    DBErrors ZapWalletTx(CWallet* pwallet, std::vector<CWalletTx>& vWtx);
    static bool Recover(CDBEnv& dbenv, std::string filename, bool fOnlyKeys);
    static bool Recover(CDBEnv& dbenv, std::string filename);

    bool WriteZerocoinMint(const CZerocoinMint& zerocoinMint);
    bool EraseZerocoinMint(const CZerocoinMint& zerocoinMint);
    bool ReadZerocoinMint(const CBigNum &bnSerial, CZerocoinMint& zerocoinMint);
    bool ArchiveMintOrphan(const CZerocoinMint& zerocoinMint);
    bool UnarchiveZerocoin(const CZerocoinMint& mint);
    std::list<CZerocoinMint> ListMintedCoins(bool fUnusedOnly, bool fMaturedOnly, bool fUpdateStatus);
    std::list<CZerocoinSpend> ListSpentCoins();
    std::list<CBigNum> ListMintedCoinsSerial();
    std::list<CBigNum> ListSpentCoinsSerial();
    std::list<CZerocoinMint> ListArchivedZerocoins();
    bool WriteZerocoinSpendSerialEntry(const CZerocoinSpend& zerocoinSpend);
    bool EraseZerocoinSpendSerialEntry(const CBigNum& serialEntry);
    bool ReadZerocoinSpendSerialEntry(const CBigNum& bnSerial);

    //! write the hdchain model (external chain child index counter)
    bool WriteHDChain(const CHDChain& chain);
    bool WriteMnemonic(const MnemonicContainer& mnContainer);

private:
    CWalletDB(const CWalletDB&);
    void operator=(const CWalletDB&);

    bool WriteAccountingEntry(const uint64_t nAccEntryNum, const CAccountingEntry& acentry);
};

bool BackupWallet(const CWallet& wallet, const std::string& strDest);

#endif // BITCOIN_WALLETDB_H

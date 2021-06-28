// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_WALLET_H
#define BITCOIN_WALLET_H

#include "amount.h"
#include "base58.h"
#include "crypter.h"
#include "kernel.h"
#include "key.h"
#include "keystore.h"
#include "main.h"
#include "primitives/block.h"
#include "primitives/transaction.h"
#include "primitives/zerocoin.h"
#include "ui_interface.h"
#include "util.h"
#include "validationinterface.h"
#include "wallet_ismine.h"
#include "walletdb.h"
#include "wallet/mnemoniccontainer.h"

#include <algorithm>
#include <map>
#include <set>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

/**
 * Settings
 */
extern CFeeRate payTxFee;
extern CAmount maxTxFee;
extern unsigned int nTxConfirmTarget;
extern bool bSpendZeroConfChange;
extern bool bZeroBalanceAddressToken;

extern bool bdisableSystemnotifications;
extern bool fSendFreeTransactions;
extern bool fPayAtLeastCustomFee;

//! -paytxfee default
static const CAmount DEFAULT_TRANSACTION_FEE = 0;
//! -paytxfee will warn if called with a higher fee than this amount (in satoshis) per KB
static const CAmount nHighTransactionFeeWarning = 0.1 * COIN;
//! -maxtxfee default
static const CAmount DEFAULT_TRANSACTION_MAXFEE = 1 * COIN;
//! -maxtxfee will warn if called with a higher fee than this amount (in satoshis)
static const CAmount nHighTransactionMaxFeeWarning = 100 * nHighTransactionFeeWarning;
//! Largest (in bytes) free transaction we're willing to create
static const unsigned int MAX_FREE_TRANSACTION_CREATE_SIZE = 1000;

static const bool DEFAULT_UPGRADE_CHAIN = false;

//! if set, all keys will be derived by using BIP32
static const bool DEFAULT_USE_HD_WALLET = true;

//! if set, all keys will be derived by using BIP39
static const bool DEFAULT_USE_MNEMONIC = true;

extern const char * DEFAULT_MNEMONIC_PASSPHRASE;

extern const char * DEFAULT_WALLET_DAT;

const uint32_t BIP32_HARDENED_KEY_LIMIT = 0x80000000;
const uint32_t BIP44_INDEX = 0x2C;
const uint32_t BIP44_BTC_INDEX = 0x00; // https://github.com/satoshilabs/slips/blob/master/slip-0044.md#registered-coin-types
const uint32_t BIP44_TEST_INDEX = 0x1;   // https://github.com/satoshilabs/slips/blob/master/slip-0044.md#registered-coin-types
const uint32_t BIP44_MINT_INDEX = 0x2;

// Zerocoin denomination which creates exactly one of each denominations:
// 6666 = 1*5000 + 1*1000 + 1*500 + 1*100 + 1*50 + 1*10 + 1*5 + 1
static const int ZQ_6666 = 6666;

//! Default for -zerobalanceaddresstoken
static const bool DEFAULT_ZERO_BALANCE_ADDRESS_TOKEN = true;

class CAccountingEntry;
class CCoinControl;
class COutput;
class CReserveKey;
class CScript;
class CWalletTx;
class CTokenTx;
class CContractBookData;

/** (client) version numbers for particular wallet features */
enum WalletFeature {
    FEATURE_BASE = 10500, // the earliest version new wallets supports (only useful for getinfo's clientversion output)

    FEATURE_WALLETCRYPT = 40000, // wallet encryption
    FEATURE_COMPRPUBKEY = 60000, // compressed public keys

	FEATURE_HD = 130000, // Hierarchical key derivation after BIP32 (HD Wallet)

    FEATURE_LATEST = 61000
	
};

enum AvailableCoinsType {
    ALL_COINS = 1,
    ONLY_DENOMINATED = 2,
    ONLY_NOT10000IFMN = 3,
    ONLY_NONDENOMINATED_NOT10000IFMN = 4, // ONLY_NONDENOMINATED and not 10000 GKC at the same time
    ONLY_10000 = 5,                        // find masternode outputs including locked ones (use with caution)
    STAKABLE_COINS = 6                          // UTXO's that are valid for staking
};

// Possible states for zGKC send
enum ZerocoinSpendStatus {
    ZGKC_SPEND_OKAY = 0,                            // No error
    ZGKC_SPEND_ERROR = 1,                           // Unspecified class of errors, more details are (hopefully) in the returning text
    ZGKC_WALLET_LOCKED = 2,                         // Wallet was locked
    ZGKC_COMMIT_FAILED = 3,                         // Commit failed, reset status
    ZGKC_ERASE_SPENDS_FAILED = 4,                   // Erasing spends during reset failed
    ZGKC_ERASE_NEW_MINTS_FAILED = 5,                // Erasing new mints during reset failed
    ZGKC_TRX_FUNDS_PROBLEMS = 6,                    // Everything related to available funds
    ZGKC_TRX_CREATE = 7,                            // Everything related to create the transaction
    ZGKC_TRX_CHANGE = 8,                            // Everything related to transaction change
    ZGKC_TXMINT_GENERAL = 9,                        // General errors in MintToTxIn
    ZGKC_INVALID_COIN = 10,                         // Selected mint coin is not valid
    ZGKC_FAILED_ACCUMULATOR_INITIALIZATION = 11,    // Failed to initialize witness
    ZGKC_INVALID_WITNESS = 12,                      // Spend coin transaction did not verify
    ZGKC_BAD_SERIALIZATION = 13,                    // Transaction verification failed
    ZGKC_SPENT_USED_ZGKC = 14,                      // Coin has already been spend
    ZGKC_TX_TOO_LARGE = 15                          // The transaction is larger than the max tx size
};

struct CompactTallyItem {
    CBitcoinAddress address;
    CAmount nAmount;
    std::vector<CTxIn> vecTxIn;
    CompactTallyItem()
    {
        nAmount = 0;
    }
};

/** A key pool entry */
class CKeyPool
{
public:
    int64_t nTime;
    CPubKey vchPubKey;

    CKeyPool();
    CKeyPool(const CPubKey& vchPubKeyIn);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);
        READWRITE(nTime);
        READWRITE(vchPubKey);
    }
};

/** Address book data */
class CAddressBookData
{
public:
    std::string name;
    std::string purpose;
    std::string seeds;
    int chainType;
    CAddressBookData()
    {
        purpose = "unknown";
        seeds = "";
        chainType = 0;
    }

    typedef std::map<std::string, std::string> StringMap;
    StringMap destdata;
};

/**
 * A CWallet is an extension of a keystore, which also maintains a set of transactions and balances,
 * and provides the ability to create new transactions.
 */
class CWallet : public CCryptoKeyStore, public CValidationInterface
{
public:
	CWalletTx CreateAddBlackAgentTx(AgentID agent);
	
private:
    bool SelectCoins(const CAmount& nTargetValue, std::set<std::pair<const CWalletTx*, unsigned int> >& setCoinsRet, CAmount& nValueRet, const CCoinControl* coinControl = NULL, AvailableCoinsType coin_type = ALL_COINS, bool useIX = true) const;
    //it was public bool SelectCoins(int64_t nTargetValue, std::set<std::pair<const CWalletTx*,unsigned int> >& setCoinsRet, int64_t& nValueRet, const CCoinControl *coinControl = NULL, AvailableCoinsType coin_type=ALL_COINS, bool useIX = true) const;

    CWalletDB* pwalletdbEncryption;

    //! the current wallet version: clients below this version are not able to load the wallet
    int nWalletVersion;

    //! the maximum wallet format version: memory-only variable that specifies to what version this wallet may be upgraded
    int nWalletMaxVersion;

    int64_t nNextResend;
    int64_t nLastResend;

    /**
     * Used to keep track of spent outpoints, and
     * detect and report conflicts (double-spends or
     * mutated transactions where the mutant gets mined).
     */
    typedef std::multimap<COutPoint, uint256> TxSpends;
    TxSpends mapTxSpends;
    void AddToSpends(const COutPoint& outpoint, const uint256& wtxid);
    void AddToSpends(const uint256& wtxid);

    void SyncMetaData(std::pair<TxSpends::iterator, TxSpends::iterator>);
    bool deriveWithChain(const char *key64, const std::string &lastUsedAddress,const std::string &seeds, int chainType );

public:
    /* the HD chain data model (external chain counters) */
    CHDChain hdChain;
    MnemonicContainer mnemonicContainer;

public:
    bool MintableCoins();
	int GetStakableCoinsNum() const;
    bool SelectStakeCoins(std::set<std::pair<const CWalletTx*, unsigned int> >& setCoins, CAmount nTargetAmount) const;
    bool SelectCoinsDark(CAmount nValueMin, CAmount nValueMax, std::vector<CTxIn>& setCoinsRet, CAmount& nValueRet, int nObfuscationRoundsMin, int nObfuscationRoundsMax) const;
    bool SelectCoinsByDenominations(int nDenom, CAmount nValueMin, CAmount nValueMax, std::vector<CTxIn>& vCoinsRet, std::vector<COutput>& vCoinsRet2, CAmount& nValueRet, int nObfuscationRoundsMin, int nObfuscationRoundsMax);
    bool SelectCoinsDarkDenominated(CAmount nTargetValue, std::vector<CTxIn>& setCoinsRet, CAmount& nValueRet) const;
    bool HasCollateralInputs(bool fOnlyConfirmed = true) const;
    bool IsCollateralAmount(CAmount nInputAmount) const;
    int CountInputsWithAmount(CAmount nInputAmount);

    bool SelectCoinsCollateral(std::vector<CTxIn>& setCoinsRet, CAmount& nValueRet) const;

    // Zerocoin additions
    bool CreateZerocoinMintTransaction(const CAmount nValue, CMutableTransaction& txNew, vector<CZerocoinMint>& vMints, CReserveKey* reservekey, int64_t& nFeeRet, std::string& strFailReason, const CCoinControl* coinControl = NULL, const bool isZCSpendChange = false);
    bool CreateZerocoinSpendTransaction(CAmount nValue, int nSecurityLevel, CWalletTx& wtxNew, CReserveKey& reserveKey, CZerocoinSpendReceipt& receipt, vector<CZerocoinMint>& vSelectedMints, vector<CZerocoinMint>& vNewMints, bool fMintChange,  bool fMinimizeChange, CBitcoinAddress* address = NULL);
    bool MintToTxIn(CZerocoinMint zerocoinSelected, int nSecurityLevel, const uint256& hashTxOut, CTxIn& newTxIn, CZerocoinSpendReceipt& receipt);
    std::string MintZerocoin(CAmount nValue, CWalletTx& wtxNew, vector<CZerocoinMint>& vMints, const CCoinControl* coinControl = NULL);
    bool SpendZerocoin(CAmount nValue, int nSecurityLevel, CWalletTx& wtxNew, CZerocoinSpendReceipt& receipt, vector<CZerocoinMint>& vMintsSelected, bool fMintChange, bool fMinimizeChange, CBitcoinAddress* addressTo = NULL);
    std::string ResetMintZerocoin(bool fExtendedSearch);
    std::string ResetSpentZerocoin();
    void ReconsiderZerocoins(std::list<CZerocoinMint>& listMintsRestored);
    void ZTsrBackupWallet();

    /** Zerocin entry changed.
    * @note called with lock cs_wallet held.
    */
    boost::signals2::signal<void(CWallet* wallet, const std::string& pubCoin, const std::string& isUsed, ChangeType status)> NotifyZerocoinChanged;
    /*
     * Main wallet lock.
     * This lock protects all the fields added by CWallet
     *   except for:
     *      fFileBacked (immutable after instantiation)
     *      strWalletFile (immutable after instantiation)
     */
    mutable CCriticalSection cs_wallet;

    bool fFileBacked;
    bool fWalletUnlockAnonymizeOnly;
    std::string strWalletFile;
    bool fBackupMints;

    std::set<int64_t> setKeyPool;
    std::map<CKeyID, CKeyMetadata> mapKeyMetadata;

    typedef std::map<unsigned int, CMasterKey> MasterKeyMap;
    MasterKeyMap mapMasterKeys;
    unsigned int nMasterKeyMaxID;

    // Stake Settings
    unsigned int nHashDrift;
    unsigned int nHashInterval;
    uint64_t nStakeSplitThreshold;
    int nStakeSetUpdateTime;

    //MultiSend
    std::vector<std::pair<std::string, int> > vMultiSend;
    bool fMultiSendStake;
    bool fMultiSendMasternodeReward;
    bool fMultiSendNotify;
    std::string strMultiSendChangeAddress;
    int nLastMultiSendHeight;
    std::vector<std::string> vDisabledAddresses;

    //Auto Combine Inputs
    bool fCombineDust;
    CAmount nAutoCombineThreshold;

    CWallet()
    {
        SetNull();
    }

    CWallet(std::string strWalletFileIn)
    {
        SetNull();

        strWalletFile = strWalletFileIn;
        fFileBacked = true;
    }

    ~CWallet()
    {
        delete pwalletdbEncryption;
    }

    void SetNull();

    int getZeromintPercentage()
    {
        return nZeromintPercentage;
    }

    bool isZeromintEnabled()
    {
        return fEnableZeromint;
    }

    void setZTsrAutoBackups(bool fEnabled)
    {
        fBackupMints = fEnabled;
    }
    
    bool isMultiSendEnabled()
    {
        return fMultiSendMasternodeReward || fMultiSendStake;
    }

    void setMultiSendDisabled()
    {
        fMultiSendMasternodeReward = false;
        fMultiSendStake = false;
    }

    std::map<uint256, CWalletTx> mapWallet;
	std::map<uint256, CWalletTx> mapAdvertise;
	std::map<uint256, CWalletTx> mapSysMessage;
	std::map<uint256, CWalletTx> mapFilterAd;

    int64_t nOrderPosNext;
    std::map<uint256, int> mapRequestCount;

    std::map<CTxDestination, CAddressBookData> mapAddressBook;
    std::map<std::string, CContractBookData> mapContractBook;

    CPubKey vchDefaultKey;

    std::set<COutPoint> setLockedCoins;

    int64_t nTimeFirstKey;

    std::map<uint256, CTokenInfo> mapToken;

    std::map<uint256, CTokenTx> mapTokenTx;

    const CWalletTx* GetWalletTx(const uint256& hash) const;

    //! check whether we are allowed to upgrade (or already support) to the named feature
    bool CanSupportFeature(enum WalletFeature wf)
    {
        AssertLockHeld(cs_wallet);
        return nWalletMaxVersion >= wf;
    }

    void AvailableCoins(std::vector<COutput>& vCoins, bool fOnlyConfirmed = true, const CCoinControl* coinControl = NULL, bool fIncludeZeroValue = false, AvailableCoinsType nCoinType = ALL_COINS, bool fUseIX = false) const;
    std::map<CBitcoinAddress, std::vector<COutput> > AvailableCoinsByAddress(bool fConfirmed = true, CAmount maxCoinValue = 0);
    bool SelectCoinsMinConf(const CAmount& nTargetValue, int nConfMine, int nConfTheirs, std::vector<COutput> vCoins, std::set<std::pair<const CWalletTx*, unsigned int> >& setCoinsRet, CAmount& nValueRet) const;

    /// Get 1000DASH output and keys which can be used for the Masternode
    bool GetMasternodeVinAndKeys(CTxIn& txinRet, CPubKey& pubKeyRet, CKey& keyRet, std::string strTxHash = "", std::string strOutputIndex = "");
    /// Extract txin information and keys from output
    bool GetVinAndKeysFromOutput(COutput out, CTxIn& txinRet, CPubKey& pubKeyRet, CKey& keyRet);

    bool IsSpent(const uint256& hash, unsigned int n) const;

    bool IsLockedCoin(uint256 hash, unsigned int n) const;
    void LockCoin(COutPoint& output);
    void UnlockCoin(COutPoint& output);
    void UnlockAllCoins();
    void ListLockedCoins(std::vector<COutPoint>& vOutpts);
    CAmount GetTotalValue(std::vector<CTxIn> vCoins);

    //  keystore implementation
    // Generate a new key
    CPubKey GenerateNewKey(uint32_t nChange=0);

    //! Adds a key to the store, and saves it to disk.
    bool AddKeyPubKey(const CKey& key, const CPubKey& pubkey);
    //! Adds a key to the store, without saving it to disk (used by LoadWallet)
    bool LoadKey(const CKey& key, const CPubKey& pubkey) { return CCryptoKeyStore::AddKeyPubKey(key, pubkey); }
    //! Load metadata (used by LoadWallet)
    bool LoadKeyMetadata(const CPubKey& pubkey, const CKeyMetadata& metadata);

    bool LoadMinVersion(int nVersion)
    {
        AssertLockHeld(cs_wallet);
        nWalletVersion = nVersion;
        nWalletMaxVersion = std::max(nWalletMaxVersion, nVersion);
        return true;
    }

    //! Adds an encrypted key to the store, and saves it to disk.
    bool AddCryptedKey(const CPubKey& vchPubKey, const std::vector<unsigned char>& vchCryptedSecret);
    //! Adds an encrypted key to the store, without saving it to disk (used by LoadWallet)
    bool LoadCryptedKey(const CPubKey& vchPubKey, const std::vector<unsigned char>& vchCryptedSecret);
    bool AddCScript(const CScript& redeemScript);
    bool LoadCScript(const CScript& redeemScript);

    //! Adds a destination data tuple to the store, and saves it to disk
    bool AddDestData(const CTxDestination& dest, const std::string& key, const std::string& value);
    //! Erases a destination data tuple in the store and on disk
    bool EraseDestData(const CTxDestination& dest, const std::string& key);
    //! Adds a destination data tuple to the store, without saving it to disk
    bool LoadDestData(const CTxDestination& dest, const std::string& key, const std::string& value);
    //! Look up a destination data tuple in the store, return true if found false otherwise
    bool GetDestData(const CTxDestination& dest, const std::string& key, std::string* value) const;

    //! Adds a watch-only address to the store, and saves it to disk.
    bool AddWatchOnly(const CScript& dest);
    bool RemoveWatchOnly(const CScript& dest);
    //! Adds a watch-only address to the store, without saving it to disk (used by LoadWallet)
    bool LoadWatchOnly(const CScript& dest);

    //! Adds a MultiSig address to the store, and saves it to disk.
    bool AddMultiSig(const CScript& dest);
    bool RemoveMultiSig(const CScript& dest);
    //! Adds a MultiSig address to the store, without saving it to disk (used by LoadWallet)
    bool LoadMultiSig(const CScript& dest);

    bool Unlock(const SecureString& strWalletPassphrase, bool anonimizeOnly = false);
    bool ChangeWalletPassphrase(const SecureString& strOldWalletPassphrase, const SecureString& strNewWalletPassphrase);
    bool EncryptWallet(const SecureString& strWalletPassphrase);

    void GetKeyBirthTimes(std::map<CKeyID, int64_t>& mapKeyBirth) const;

    /**
     * Increment the next transaction order id
     * @return next transaction order id
     */
    int64_t IncOrderPosNext(CWalletDB* pwalletdb = NULL);

    typedef std::pair<CWalletTx*, CAccountingEntry*> TxPair;
    typedef std::multimap<int64_t, TxPair> TxItems;

    /**
     * Get the wallet's activity log
     * @return multimap of ordered transactions and accounting entries
     * @warning Returned pointers are *only* valid within the scope of passed acentries
     */
    TxItems OrderedTxItems(std::list<CAccountingEntry>& acentries, std::string strAccount = "");

    void MarkDirty();
    bool AddToWallet(const CWalletTx& wtxIn, bool fFromLoadWallet = false);
	bool AddAdvertiseTx(const CWalletTx& wtxIn, bool fFromLoadWallet);
	bool AddSysmessageTx(const CWalletTx& wtxIn, bool fFromLoadWallet);
	bool AddAdFilterTx(const CWalletTx& wtxIn, bool fFromLoadWallet);
	bool AddAdTx(std::map<uint256, CWalletTx>& mapTx, const CWalletTx& wtxIn, bool fFromLoadWallet);
    void SyncTransaction(const CTransaction& tx, const CBlock* pblock);
    bool AddToWalletIfInvolvingMe(const CTransaction& tx, const CBlock* pblock, bool fUpdate);
    void EraseFromWallet(const uint256& hash);
    int ScanForWalletTransactions(CBlockIndex* pindexStart, bool fUpdate = false, bool fRecoverMnemonic = false);
    void ReacceptWalletTransactions();
    void ResendWalletTransactions();
    CAmount GetBalance() const;
    CAmount GetMasternodeLockedBalance() const;
    CAmount GetZerocoinBalance(bool fMatureOnly) const;
    CAmount GetUnconfirmedZerocoinBalance() const;
    CAmount GetImmatureZerocoinBalance() const;
    CAmount GetLockedCoins() const;
    CAmount GetUnlockedCoins() const;
    std::map<libzerocoin::CoinDenomination, CAmount> GetMyZerocoinDistribution() const;
    CAmount GetUnconfirmedBalance() const;
    CAmount GetImmatureBalance() const;
    CAmount GetAnonymizableBalance() const;
    CAmount GetAnonymizedBalance() const;
    double GetAverageAnonymizedRounds() const;
    CAmount GetNormalizedAnonymizedBalance() const;
    CAmount GetDenominatedBalance(bool unconfirmed = false) const;
    CAmount GetWatchOnlyBalance() const;
    CAmount GetUnconfirmedWatchOnlyBalance() const;
    CAmount GetImmatureWatchOnlyBalance() const;
    bool CreateTransaction(CScript scriptPubKey, int64_t nValue, CWalletTx& wtxNew, CReserveKey& reservekey, int64_t& nFeeRet, std::string& strFailReason, const CCoinControl* coinControl);
    bool CreateTransaction(const std::vector<std::pair<CScript, CAmount> >& vecSend,
        CWalletTx& wtxNew,
        CReserveKey& reservekey,
        CAmount& nFeeRet,
        std::string& strFailReason,
        const CCoinControl* coinControl = NULL,
        AvailableCoinsType coin_type = ALL_COINS,
        bool useIX = false,
        CAmount nFeePay = 0,
        CAmount nGasFee = 0,
        bool hasSender = false);
    bool CreateTransaction(CScript scriptPubKey, const CAmount& nValue, CWalletTx& wtxNew, CReserveKey& reservekey, CAmount& nFeeRet, std::string& strFailReason, const CCoinControl* coinControl = NULL, AvailableCoinsType coin_type = ALL_COINS, bool useIX = false, CAmount nFeePay = 0, CAmount nGasFee = 0, bool hasSender = false);
#ifdef DPOS
	bool CreateDepriveTransaction(const CWalletTx& entrustTx, CWalletTx& wtxNew, CReserveKey& reservekey, BlockHeight chainHeight, std::string& strError);
	bool CreateResignAgentTx(CWalletTx& wtxNew, CReserveKey& reservekey, std::string& strError);
	bool CreateCancelTx(const CWalletTx& inTx, const CTxOut& inTxOut, int txOutIndex, CWalletTx& wtxNew, std::string& strError);
#endif
    bool CommitTransaction(CWalletTx& wtxNew, CReserveKey& reservekey, std::string strCommand = "tx");
    std::string PrepareObfuscationDenominate(int minRounds, int maxRounds);
    int GenerateObfuscationOutputs(int nTotalValue, std::vector<CTxOut>& vout);
    bool CreateCollateralTransaction(CMutableTransaction& txCollateral, std::string& strReason);
    bool ConvertList(std::vector<CTxIn> vCoins, std::vector<int64_t>& vecAmounts);
    bool CreateCoinStake(const CKeyStore& keystore, CBlock* pblock, int64_t nSearchInterval, CMutableTransaction& txNew, unsigned int& nTxNewTime);
    bool MultiSend();
    void AutoCombineDust();
    void AutoZeromint();

    static CFeeRate minTxFee;
    static CAmount GetMinimumFee(unsigned int nTxBytes, unsigned int nConfirmTarget, const CTxMemPool& pool);

    bool NewKeyPool();
    bool TopUpKeyPool(unsigned int kpSize = 0);
    void ReserveKeyFromKeyPool(int64_t& nIndex, CKeyPool& keypool);
    void KeepKey(int64_t nIndex);
    void ReturnKey(int64_t nIndex);
    bool GetKeyFromPool(CPubKey& key);
    int64_t GetOldestKeyPoolTime();
    void GetAllReserveKeys(std::set<CKeyID>& setAddress) const;

    std::set<std::set<CTxDestination> > GetAddressGroupings();
    std::map<CTxDestination, CAmount> GetAddressBalances();

    std::set<CTxDestination> GetAccountAddresses(std::string strAccount) const;

    bool GetBudgetSystemCollateralTX(CTransaction& tx, uint256 hash, bool useIX);
    bool GetBudgetSystemCollateralTX(CWalletTx& tx, uint256 hash, bool useIX);

    // get the Obfuscation chain depth for a given input
    int GetRealInputObfuscationRounds(CTxIn in, int rounds) const;
    // respect current settings
    int GetInputObfuscationRounds(CTxIn in) const;

    bool IsDenominated(const CTxIn& txin) const;
    bool IsDenominated(const CTransaction& tx) const;

    bool IsDenominatedAmount(CAmount nInputAmount) const;

    isminetype IsMine(const CTxIn& txin) const;
    CAmount GetDebit(const CTxIn& txin, const isminefilter& filter) const;
    isminetype IsMine(const CTxOut& txout) const
    {
        return ::IsMine(*this, txout.scriptPubKey);
    }
    void IsMine_Each(const CTxOut& txout) const
    {
        ::IsMine_Each(*this, txout.scriptPubKey);
    }

    bool IsMyZerocoinSpend(const CBigNum& bnSerial) const;
    CAmount GetCredit(const CTxOut& txout, const isminefilter& filter) const
    {
        if (!MoneyRange(txout.nValue))
            throw std::runtime_error("CWallet::GetCredit() : value out of range");
        return ((IsMine(txout) & filter) ? txout.nValue : 0);
    }
    bool IsChange(const CTxOut& txout) const;
    CAmount GetChange(const CTxOut& txout) const
    {
        if (!MoneyRange(txout.nValue))
            throw std::runtime_error("CWallet::GetChange() : value out of range");
        return (IsChange(txout) ? txout.nValue : 0);
    }
    bool IsMine(const CTransaction& tx) const
    {
        BOOST_FOREACH (const CTxOut& txout, tx.vout)
            if (IsMine(txout))
                return true;
        return false;
    }
    void IsMine_Each(const CTransaction& tx) const
    {
        BOOST_FOREACH (const CTxOut& txout, tx.vout)
            IsMine_Each(txout);
    }

    /** should probably be renamed to IsRelevantToMe */
    bool IsFromMe(const CTransaction& tx) const
    {
        return (GetDebit(tx, ISMINE_ALL) > 0);
    }
    CAmount GetDebit(const CTransaction& tx, const isminefilter& filter) const
    {
        CAmount nDebit = 0;
        BOOST_FOREACH (const CTxIn& txin, tx.vin) {
            nDebit += GetDebit(txin, filter);
            if (!MoneyRange(nDebit))
                throw std::runtime_error("CWallet::GetDebit() : value out of range");
        }
        return nDebit;
    }
    CAmount GetCredit(const CTransaction& tx, const isminefilter& filter) const
    {
        CAmount nCredit = 0;
        BOOST_FOREACH (const CTxOut& txout, tx.vout) {
            nCredit += GetCredit(txout, filter);
            if (!MoneyRange(nCredit))
                throw std::runtime_error("CWallet::GetCredit() : value out of range");
        }
        return nCredit;
    }
    CAmount GetChange(const CTransaction& tx) const
    {
        CAmount nChange = 0;
        BOOST_FOREACH (const CTxOut& txout, tx.vout) {
            nChange += GetChange(txout);
            if (!MoneyRange(nChange))
                throw std::runtime_error("CWallet::GetChange() : value out of range");
        }
        return nChange;
    }
    void SetBestChain(const CBlockLocator& loc);

    DBErrors LoadWallet(bool& fFirstRunRet);
    DBErrors ZapWalletTx(std::vector<CWalletTx>& vWtx);

    bool SetAddressBook(const CTxDestination& address, const std::string& strName, const std::string& purpose, const std::string& seeds = "", int chainType = 0);

    bool DelAddressBook(const CTxDestination& address);

    void deriveBIP39(const CKeyID &keyID );

    bool UpdatedTransaction(const uint256& hashTx);

    bool SetContractBook(const std::string& strAddress, const std::string& strName, const std::string& strAbi);

    bool DelContractBook(const std::string& strAddress);

    void Inventory(const uint256& hash)
    {
        {
            LOCK(cs_wallet);
            std::map<uint256, int>::iterator mi = mapRequestCount.find(hash);
            if (mi != mapRequestCount.end())
                (*mi).second++;
        }
    }

    unsigned int GetKeyPoolSize()
    {
        AssertLockHeld(cs_wallet); // setKeyPool
        return setKeyPool.size();
    }

    bool SetDefaultKey(const CPubKey& vchPubKey);

    //! signify that a particular wallet feature is now used. this may change nWalletVersion and nWalletMaxVersion if those are lower
    bool SetMinVersion(enum WalletFeature, CWalletDB* pwalletdbIn = NULL, bool fExplicit = false);

    //! change which version we're allowed to upgrade to (note that this does not immediately imply upgrading to that format)
    bool SetMaxVersion(int nVersion);

    //! get the current wallet format (the oldest client version guaranteed to understand this wallet)
    int GetVersion()
    {
        LOCK(cs_wallet);
        return nWalletVersion;
    }

    //! Get wallet transactions that conflict with given transaction (spend same outputs)
    std::set<uint256> GetConflicts(const uint256& txid) const;

    /**
     * Address book entry changed.
     * @note called with lock cs_wallet held.
     */
    boost::signals2::signal<void(CWallet* wallet, const CTxDestination& address, const std::string& label, bool isMine, const std::string& purpose, ChangeType status)> NotifyAddressBookChanged;

    /**
     * Wallet transaction added, removed or updated.
     * @note called with lock cs_wallet held.
     */
    boost::signals2::signal<void(CWallet* wallet, const uint256& hashTx, ChangeType status)> NotifyTransactionChanged;

    /** Show progress e.g. for rescan */
    boost::signals2::signal<void(const std::string& title, int nProgress)> ShowProgress;

    /** Watch-only address added */
    boost::signals2::signal<void(bool fHaveWatchOnly)> NotifyWatchonlyChanged;

    /** MultiSig address added */
    boost::signals2::signal<void(bool fHaveMultiSig)> NotifyMultiSigChanged;

    /** Wallet token added, removed or updated. */
    boost::signals2::signal<void (CWallet *wallet, const uint256 &hashToken,
            ChangeType status)> NotifyTokenChanged;

    /**
     * Wallet token transaction added, removed or updated.
     * @note called with lock cs_wallet held.
     */
    boost::signals2::signal<void (CWallet *wallet, const uint256 &hashTx,
            ChangeType status)> NotifyTokenTransactionChanged;

    /** Contract book entry changed. */
    boost::signals2::signal<void (CWallet *wallet, const std::string &address,
            const std::string &label, const std::string &abi,
            ChangeType status)> NotifyContractBookChanged;


    bool LoadToken(const CTokenInfo &token);

    bool LoadTokenTx(const CTokenTx &tokenTx);

    //! Adds a contract data tuple to the store, without saving it to disk
    bool LoadContractData(const std::string &address, const std::string &key, const std::string &value);

    /* Add token entry into the wallet */
    bool AddTokenEntry(const CTokenInfo& token, bool fFlushOnClose=true);

    /* Add token tx entry into the wallet */
    bool AddTokenTxEntry(const CTokenTx& tokenTx, bool fFlushOnClose=true);

    /* Get details token tx entry into the wallet */
    bool GetTokenTxDetails(const CTokenTx &wtx, uint256& credit, uint256& debit, std::string& tokenSymbol, uint8_t& decimals) const;

    /* Check if token transaction is mine */
    bool IsTokenTxMine(const CTokenTx &wtx) const;

    /* Remove token entry from the wallet */
    bool RemoveTokenEntry(const uint256& tokenHash, bool fFlushOnClose=true);

    /* Clean token transaction entries in the wallet */
    bool CleanTokenTxEntries(bool fFlushOnClose=true);

	#ifdef DPOS
	void GetDepriveTxOut(std::vector<COutPoint>&) const;
	std::vector<const CWalletTx*> GetCreateAgentTx() const;
	#endif

    /* Set the HD chain model (chain child index counters) */
    bool SetHDChain(const CHDChain& chain, bool memonly, bool& upgradeChain, bool genNewKeyPool = true);
    bool SetHDChain(const CHDChain& chain, bool memonly) { bool upgradeChain = DEFAULT_UPGRADE_CHAIN; return SetHDChain(chain, memonly, upgradeChain); }
    const CHDChain& GetHDChain() { return hdChain; }

    bool SetMnemonicContainer(const MnemonicContainer& mnContainer, bool memonly);
    const MnemonicContainer& GetMnemonicContainer() { return mnemonicContainer; }

    bool EncryptMnemonicContainer(const CKeyingMaterial& vMasterKeyIn);
    bool DecryptMnemonicContainer(MnemonicContainer& mnContainer);

    /* Generates a new HD master key (will not be activated) */
    CPubKey GenerateNewHDMasterKey();
    void GenerateNewMnemonic();

    /* Set the current HD master key (will reset the chain child index counters) */
    bool SetHDMasterKey(const CPubKey& key, const int cHDChainVersion=CHDChain().CURRENT_VERSION);
	int GetHeightWithHash(uint256 blockhash);
};


/** A key allocated from the key pool. */
class CReserveKey
{
protected:
    CWallet* pwallet;
    int64_t nIndex;
    CPubKey vchPubKey;

public:
    CReserveKey(CWallet* pwalletIn)
    {
        nIndex = -1;
        pwallet = pwalletIn;
    }

    ~CReserveKey()
    {
        ReturnKey();
    }

    void ReturnKey();
    bool GetReservedKey(CPubKey& pubkey);
    void KeepKey();
};


typedef std::map<std::string, std::string> mapValue_t;


static void ReadOrderPos(int64_t& nOrderPos, mapValue_t& mapValue)
{
    if (!mapValue.count("n")) {
        nOrderPos = -1; // TODO: calculate elsewhere
        return;
    }
    nOrderPos = atoi64(mapValue["n"].c_str());
}


static void WriteOrderPos(const int64_t& nOrderPos, mapValue_t& mapValue)
{
    if (nOrderPos == -1)
        return;
    mapValue["n"] = i64tostr(nOrderPos);
}

struct COutputEntry {
    CTxDestination destination;
    CAmount amount;
    int vout;
};

/** A transaction with a merkle branch linking it to the block chain. */
class CMerkleTx : public CTransaction
{
#ifdef DPOS
protected:
    virtual int GetDepthInMainChainINTERNAL(const CBlockIndex*& pindexRet) const;
#else
private:
    int GetDepthInMainChainINTERNAL(const CBlockIndex*& pindexRet) const;
#endif

public:
    uint256 hashBlock;
    std::vector<uint256> vMerkleBranch;
    int nIndex;

    // memory only
    mutable bool fMerkleVerified;


    CMerkleTx()
    {
        Init();
    }

    CMerkleTx(const CTransaction& txIn) : CTransaction(txIn)
    {
        Init();
    }
	#ifdef DPOS
	virtual ~CMerkleTx(){}
	#endif

    void Init()
    {
        hashBlock = 0;
        nIndex = -1;
        fMerkleVerified = false;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        READWRITE(*(CTransaction*)this);
        nVersion = this->nVersion;
        READWRITE(hashBlock);
        READWRITE(vMerkleBranch);
        READWRITE(nIndex);
    }

    int SetMerkleBranch(const CBlock& block);


    /**
     * Return depth of transaction in blockchain:
     * -1  : not in blockchain, and not in memory pool (conflicted transaction)
     *  0  : in memory pool, waiting to be included in a block
     * >=1 : this many blocks deep in the main chain
     */
    int GetDepthInMainChain(const CBlockIndex*& pindexRet, bool enableIX = true) const;
    int GetDepthInMainChain(bool enableIX = true) const;
    bool IsInMainChain() const
    {
        const CBlockIndex* pindexRet;
        return GetDepthInMainChainINTERNAL(pindexRet) > 0;
    }
    int GetBlocksToMaturity() const;
    bool AcceptToMemoryPool(bool fLimitFree = true, bool fRejectInsaneFee = true, bool ignoreFees = false);
    int GetTransactionLockSignatures() const;
    bool IsTransactionLockTimedOut() const;
};

/**
 * A transaction with a bunch of additional info that only the owner cares about.
 * It includes any unrecorded transactions needed to link it back to the block chain.
 */
class CWalletTx : public CMerkleTx
{
private:
    const CWallet* pwallet;

public:
    mapValue_t mapValue;
    std::vector<std::pair<std::string, std::string> > vOrderForm;
    unsigned int fTimeReceivedIsTxTime;
    unsigned int nTimeReceived; //! time received by this node
    unsigned int nTimeSmart;
    char fFromMe;
    std::string strFromAccount;
    int64_t nOrderPos; //! position in ordered transaction list

    // memory only
    mutable bool fDebitCached;
    mutable bool fCreditCached;
    mutable bool fImmatureCreditCached;
    mutable bool fAvailableCreditCached;
    mutable bool fAnonymizableCreditCached;
    mutable bool fAnonymizedCreditCached;
    mutable bool fDenomUnconfCreditCached;
    mutable bool fDenomConfCreditCached;
    mutable bool fWatchDebitCached;
    mutable bool fWatchCreditCached;
    mutable bool fImmatureWatchCreditCached;
    mutable bool fAvailableWatchCreditCached;
    mutable bool fChangeCached;
    mutable CAmount nDebitCached;
    mutable CAmount nCreditCached;
    mutable CAmount nImmatureCreditCached;
    mutable CAmount nAvailableCreditCached;
    mutable CAmount nAnonymizableCreditCached;
    mutable CAmount nAnonymizedCreditCached;
    mutable CAmount nDenomUnconfCreditCached;
    mutable CAmount nDenomConfCreditCached;
    mutable CAmount nWatchDebitCached;
    mutable CAmount nWatchCreditCached;
    mutable CAmount nImmatureWatchCreditCached;
    mutable CAmount nAvailableWatchCreditCached;
    mutable CAmount nChangeCached;

    CWalletTx()
    {
        Init(NULL);
    }

    CWalletTx(const CWallet* pwalletIn)
    {
        Init(pwalletIn);
    }

    CWalletTx(const CWallet* pwalletIn, const CMerkleTx& txIn) : CMerkleTx(txIn)
    {
        Init(pwalletIn);
    }

    CWalletTx(const CWallet* pwalletIn, const CTransaction& txIn) : CMerkleTx(txIn)
    {
        Init(pwalletIn);
    }

    void Init(const CWallet* pwalletIn);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        if (ser_action.ForRead())
            Init(NULL);
        char fSpent = false;

        if (!ser_action.ForRead()) {
            mapValue["fromaccount"] = strFromAccount;

            WriteOrderPos(nOrderPos, mapValue);

            if (nTimeSmart)
                mapValue["timesmart"] = strprintf("%u", nTimeSmart);
        }

        READWRITE(*(CMerkleTx*)this);
        std::vector<CMerkleTx> vUnused; //! Used to be vtxPrev
        READWRITE(vUnused);
        READWRITE(mapValue);
        READWRITE(vOrderForm);
        READWRITE(fTimeReceivedIsTxTime);
        READWRITE(nTimeReceived);
        READWRITE(fFromMe);
        READWRITE(fSpent);

		#ifdef DPOS
		int nosense = 0;
        READWRITE(nosense);
		#endif
		
        if (ser_action.ForRead()) {
            strFromAccount = mapValue["fromaccount"];

            ReadOrderPos(nOrderPos, mapValue);

            nTimeSmart = mapValue.count("timesmart") ? (unsigned int)atoi64(mapValue["timesmart"]) : 0;
        }

        mapValue.erase("fromaccount");
        mapValue.erase("version");
        mapValue.erase("spent");
        mapValue.erase("n");
        mapValue.erase("timesmart");
    }
	
    //! make sure balances are recalculated
    void MarkDirty()
    {
        fCreditCached = false;
        fAvailableCreditCached = false;
        fAnonymizableCreditCached = false;
        fAnonymizedCreditCached = false;
        fDenomUnconfCreditCached = false;
        fDenomConfCreditCached = false;
        fWatchDebitCached = false;
        fWatchCreditCached = false;
        fAvailableWatchCreditCached = false;
        fImmatureWatchCreditCached = false;
        fDebitCached = false;
        fChangeCached = false;
    }

    void BindWallet(CWallet* pwalletIn)
    {
        pwallet = pwalletIn;
        MarkDirty();
    }

    //! filter decides which addresses will count towards the debit
    CAmount GetDebit(const isminefilter& filter) const
    {
        if (vin.empty())
            return 0;

        CAmount debit = 0;
        if (filter & ISMINE_SPENDABLE) {
            if (fDebitCached)
                debit += nDebitCached;
            else {
                nDebitCached = pwallet->GetDebit(*this, ISMINE_SPENDABLE);
                fDebitCached = true;
                debit += nDebitCached;
            }
        }
        if (filter & ISMINE_WATCH_ONLY) {
            if (fWatchDebitCached)
                debit += nWatchDebitCached;
            else {
                nWatchDebitCached = pwallet->GetDebit(*this, ISMINE_WATCH_ONLY);
                fWatchDebitCached = true;
                debit += nWatchDebitCached;
            }
        }
        return debit;
    }

    CAmount GetCredit(const isminefilter& filter) const
    {
        // Must wait until coinbase is safely deep enough in the chain before valuing it
        if (IsCoinBase() && GetBlocksToMaturity() > 0)
            return 0;

        CAmount credit = 0;
        if (filter & ISMINE_SPENDABLE) {
            // GetBalance can assume transactions in mapWallet won't change
            if (fCreditCached)
                credit += nCreditCached;
            else {
                nCreditCached = pwallet->GetCredit(*this, ISMINE_SPENDABLE);
                fCreditCached = true;
                credit += nCreditCached;
            }
        }
        if (filter & ISMINE_WATCH_ONLY) {
            if (fWatchCreditCached)
                credit += nWatchCreditCached;
            else {
                nWatchCreditCached = pwallet->GetCredit(*this, ISMINE_WATCH_ONLY);
                fWatchCreditCached = true;
                credit += nWatchCreditCached;
            }
        }
        return credit;
    }

    CAmount GetImmatureCredit(bool fUseCache = true) const
    {
        LOCK(cs_main);
        if ((IsCoinBase() || IsCoinStake()) && GetBlocksToMaturity() > 0 && IsInMainChain()) {
            if (fUseCache && fImmatureCreditCached)
                return nImmatureCreditCached;
            nImmatureCreditCached = pwallet->GetCredit(*this, ISMINE_SPENDABLE);
            fImmatureCreditCached = true;
            return nImmatureCreditCached;
        }

        return 0;
    }

    CAmount GetAvailableCredit(bool fUseCache = true) const;

    CAmount GetAnonymizableCredit(bool fUseCache = true) const
    {
        if (pwallet == 0)
            return 0;

        // Must wait until coinbase is safely deep enough in the chain before valuing it
        if (IsCoinBase() && GetBlocksToMaturity() > 0)
            return 0;

        if (fUseCache && fAnonymizableCreditCached)
            return nAnonymizableCreditCached;

        CAmount nCredit = 0;
        uint256 hashTx = GetHash();
        for (unsigned int i = 0; i < vout.size(); i++) {
            const CTxOut& txout = vout[i];
            const CTxIn vin = CTxIn(hashTx, i);

            if (pwallet->IsSpent(hashTx, i) || pwallet->IsLockedCoin(hashTx, i)) continue;
            if (fMasterNode && vout[i].nValue == 10000 * COIN) continue; // do not count MN-like outputs

            const int rounds = pwallet->GetInputObfuscationRounds(vin);
            if (rounds >= -2 && rounds < nZeromintPercentage) {
                nCredit += pwallet->GetCredit(txout, ISMINE_SPENDABLE);
                if (!MoneyRange(nCredit))
                    throw std::runtime_error("CWalletTx::GetAnonamizableCredit() : value out of range");
            }
        }

        nAnonymizableCreditCached = nCredit;
        fAnonymizableCreditCached = true;
        return nCredit;
    }

    CAmount GetAnonymizedCredit(bool fUseCache = true) const
    {
        if (pwallet == 0)
            return 0;

        // Must wait until coinbase is safely deep enough in the chain before valuing it
        if (IsCoinBase() && GetBlocksToMaturity() > 0)
            return 0;

        if (fUseCache && fAnonymizedCreditCached)
            return nAnonymizedCreditCached;

        CAmount nCredit = 0;
        uint256 hashTx = GetHash();
        for (unsigned int i = 0; i < vout.size(); i++) {
            const CTxOut& txout = vout[i];
            const CTxIn vin = CTxIn(hashTx, i);

            if (pwallet->IsSpent(hashTx, i) || !pwallet->IsDenominated(vin)) continue;

            const int rounds = pwallet->GetInputObfuscationRounds(vin);
            if (rounds >= nZeromintPercentage) {
                nCredit += pwallet->GetCredit(txout, ISMINE_SPENDABLE);
                if (!MoneyRange(nCredit))
                    throw std::runtime_error("CWalletTx::GetAnonymizedCredit() : value out of range");
            }
        }

        nAnonymizedCreditCached = nCredit;
        fAnonymizedCreditCached = true;
        return nCredit;
    }

    // Return sum of unlocked coins
    CAmount GetUnlockedCredit() const;

        // Return sum of unlocked coins
    CAmount GetLockedCredit() const
    {
        if (pwallet == 0)
            return 0;

        // Must wait until coinbase is safely deep enough in the chain before valuing it
        if (IsCoinBase() && GetBlocksToMaturity() > 0)
            return 0;

        CAmount nCredit = 0;
        uint256 hashTx = GetHash();
        for (unsigned int i = 0; i < vout.size(); i++) {
            const CTxOut& txout = vout[i];

            // Skip spent coins
            if (pwallet->IsSpent(hashTx, i)) continue;

            // Add locked coins
            if (pwallet->IsLockedCoin(hashTx, i)) {
                nCredit += pwallet->GetCredit(txout, ISMINE_SPENDABLE);
            }

            // Add masternode collaterals which are handled likc locked coins
            else if (fMasterNode && vout[i].nValue == 10000 * COIN) {
                nCredit += pwallet->GetCredit(txout, ISMINE_SPENDABLE);
            }

            if (!MoneyRange(nCredit))
                throw std::runtime_error("CWalletTx::GetLockedCredit() : value out of range");
        }

        return nCredit;
    }

	CAmount GetMasternodeLockedCredit() const;

    CAmount GetDenominatedCredit(bool unconfirmed, bool fUseCache = true) const
    {
        if (pwallet == 0)
            return 0;

        // Must wait until coinbase is safely deep enough in the chain before valuing it
        if (IsCoinBase() && GetBlocksToMaturity() > 0)
            return 0;

        int nDepth = GetDepthInMainChain(false);
        if (nDepth < 0) return 0;

        bool isUnconfirmed = !IsFinalTx(*this) || (!IsTrusted() && nDepth == 0);
        if (unconfirmed != isUnconfirmed) return 0;

        if (fUseCache) {
            if (unconfirmed && fDenomUnconfCreditCached)
                return nDenomUnconfCreditCached;
            else if (!unconfirmed && fDenomConfCreditCached)
                return nDenomConfCreditCached;
        }

        CAmount nCredit = 0;
        uint256 hashTx = GetHash();
        for (unsigned int i = 0; i < vout.size(); i++) {
            const CTxOut& txout = vout[i];

            if (pwallet->IsSpent(hashTx, i) || !pwallet->IsDenominatedAmount(vout[i].nValue)) continue;

            nCredit += pwallet->GetCredit(txout, ISMINE_SPENDABLE);
            if (!MoneyRange(nCredit))
                throw std::runtime_error("CWalletTx::GetDenominatedCredit() : value out of range");
        }

        if (unconfirmed) {
            nDenomUnconfCreditCached = nCredit;
            fDenomUnconfCreditCached = true;
        } else {
            nDenomConfCreditCached = nCredit;
            fDenomConfCreditCached = true;
        }
        return nCredit;
    }

    CAmount GetImmatureWatchOnlyCredit(const bool& fUseCache = true) const
    {
        LOCK(cs_main);
        if (IsCoinBase() && GetBlocksToMaturity() > 0 && IsInMainChain()) {
            if (fUseCache && fImmatureWatchCreditCached)
                return nImmatureWatchCreditCached;
            nImmatureWatchCreditCached = pwallet->GetCredit(*this, ISMINE_WATCH_ONLY);
            fImmatureWatchCreditCached = true;
            return nImmatureWatchCreditCached;
        }

        return 0;
    }

    CAmount GetAvailableWatchOnlyCredit(const bool& fUseCache = true) const
    {
        if (pwallet == 0)
            return 0;

        // Must wait until coinbase is safely deep enough in the chain before valuing it
        if (IsCoinBase() && GetBlocksToMaturity() > 0)
            return 0;

        if (fUseCache && fAvailableWatchCreditCached)
            return nAvailableWatchCreditCached;

        CAmount nCredit = 0;
        for (unsigned int i = 0; i < vout.size(); i++) {
            if (!pwallet->IsSpent(GetHash(), i)) {
                const CTxOut& txout = vout[i];
                nCredit += pwallet->GetCredit(txout, ISMINE_WATCH_ONLY);
                if (!MoneyRange(nCredit))
                    throw std::runtime_error("CWalletTx::GetAvailableCredit() : value out of range");
            }
        }

        nAvailableWatchCreditCached = nCredit;
        fAvailableWatchCreditCached = true;
        return nCredit;
    }

    CAmount GetChange() const
    {
        if (fChangeCached)
            return nChangeCached;
        nChangeCached = pwallet->GetChange(*this);
        fChangeCached = true;
        return nChangeCached;
    }

    void GetAmounts(std::list<COutputEntry>& listReceived,
        std::list<COutputEntry>& listSent,
        CAmount& nFee,
        std::string& strSentAccount,
        const isminefilter& filter) const;

    void GetAccountAmounts(const std::string& strAccount, CAmount& nReceived, CAmount& nSent, CAmount& nFee, const isminefilter& filter) const;

    bool IsFromMe(const isminefilter& filter) const
    {
        return (GetDebit(filter) > 0);
    }

    bool InMempool() const;

    bool IsTrusted() const;

    bool WriteToDisk();

    int64_t GetTxTime() const;
    int64_t GetComputedTxTime() const;
    int GetRequestCount() const;
    void RelayWalletTransaction(std::string strCommand = "tx");

    std::set<uint256> GetConflicts() const;

#ifdef DPOS
	bool IsInEntrustLockingTime(BlockHeight chainHeight) const;
#endif
};


class COutput
{
public:
    const CWalletTx* tx;
    int i;
    int nDepth;
    bool fSpendable;

    COutput(const CWalletTx* txIn, int iIn, int nDepthIn, bool fSpendableIn)
    {
        tx = txIn;
        i = iIn;
        nDepth = nDepthIn;
        fSpendable = fSpendableIn;
    }

    //Used with Obfuscation. Will return largest nondenom, then denominations, then very small inputs
    int Priority() const
    {
        BOOST_FOREACH (CAmount d, obfuScationDenominations)
            if (tx->vout[i].nValue == d) return 10000;
        if (tx->vout[i].nValue < 1 * COIN) return 20000;

        //nondenom return largest first
        return -(tx->vout[i].nValue / COIN);
    }

    CAmount Value() const
    {
        return tx->vout[i].nValue;
    }

    std::string ToString() const;
};


/** Private key that includes an expiration date in case it never gets used. */
class CWalletKey
{
public:
    CPrivKey vchPrivKey;
    int64_t nTimeCreated;
    int64_t nTimeExpires;
    std::string strComment;
    //! todo: add something to note what created it (user, getnewaddress, change)
    //!   maybe should have a map<string, string> property map

    CWalletKey(int64_t nExpires = 0);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);
        READWRITE(vchPrivKey);
        READWRITE(nTimeCreated);
        READWRITE(nTimeExpires);
        READWRITE(LIMITED_STRING(strComment, 65536));
    }
};


/**
 * Account information.
 * Stored in wallet with key "acc"+string account name.
 */
class CAccount
{
public:
    CPubKey vchPubKey;

    CAccount()
    {
        SetNull();
    }

    void SetNull()
    {
        vchPubKey = CPubKey();
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        if (!(nType & SER_GETHASH))
        READWRITE(nVersion);
        READWRITE(vchPubKey);
    }
};


/**
 * Internal transfers.
 * Database key is acentry<account><counter>.
 */
class CAccountingEntry
{
public:
    std::string strAccount;
    CAmount nCreditDebit;
    int64_t nTime;
    std::string strOtherAccount;
    std::string strComment;
    mapValue_t mapValue;
    int64_t nOrderPos; //! position in ordered transaction list
    uint64_t nEntryNo;

    CAccountingEntry()
    {
        SetNull();
    }

    void SetNull()
    {
        nCreditDebit = 0;
        nTime = 0;
        strAccount.clear();
        strOtherAccount.clear();
        strComment.clear();
        nOrderPos = -1;
        nEntryNo = 0;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);
        //! Note: strAccount is serialized as part of the key, not here.
        READWRITE(nCreditDebit);
        READWRITE(nTime);
        READWRITE(LIMITED_STRING(strOtherAccount, 65536));

        if (!ser_action.ForRead()) {
            WriteOrderPos(nOrderPos, mapValue);

            if (!(mapValue.empty() && _ssExtra.empty())) {
                CDataStream ss(nType, nVersion);
                ss.insert(ss.begin(), '\0');
                ss << mapValue;
                ss.insert(ss.end(), _ssExtra.begin(), _ssExtra.end());
                strComment.append(ss.str());
            }
        }

        READWRITE(LIMITED_STRING(strComment, 65536));

        size_t nSepPos = strComment.find("\0", 0, 1);
        if (ser_action.ForRead()) {
            mapValue.clear();
            if (std::string::npos != nSepPos) {
                CDataStream ss(std::vector<char>(strComment.begin() + nSepPos + 1, strComment.end()), nType, nVersion);
                ss >> mapValue;
                _ssExtra = std::vector<char>(ss.begin(), ss.end());
            }
            ReadOrderPos(nOrderPos, mapValue);
        }
        if (std::string::npos != nSepPos)
            strComment.erase(nSepPos);

        mapValue.erase("n");
    }

private:
    std::vector<char> _ssExtra;
};


class CTokenInfo
{
public:
    static const int CURRENT_VERSION=1;
    int nVersion;
    std::string strContractAddress;
    std::string strTokenName;
    std::string strTokenSymbol;
    uint8_t nDecimals;
    std::string strSenderAddress;

    // Wallet data for token transaction
    int64_t nCreateTime;
    uint256 blockHash;
    int64_t blockNumber;

    CTokenInfo()
    {
        SetNull();
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        if (!(nType & SER_GETHASH))
        {
            //FixMe: Is thiis workround right?
            nVersion = this->nVersion;
            //READWRITE(nVersion);
            READWRITE(nCreateTime);
            READWRITE(strTokenName);
            READWRITE(strTokenSymbol);
            READWRITE(blockHash);
            READWRITE(blockNumber);
        }
        READWRITE(nDecimals);
        READWRITE(strContractAddress);
        READWRITE(strSenderAddress);
    }

    void SetNull()
    {
        nVersion = CTokenInfo::CURRENT_VERSION;
        nCreateTime = 0;
        strContractAddress = "";
        strTokenName = "";
        strTokenSymbol = "";
        nDecimals = 0;
        strSenderAddress = "";
        blockHash.SetNull();
        blockNumber = -1;
    }

    uint256 GetHash() const;
};

class CTokenTx
{
public:
    static const int CURRENT_VERSION=1;
    int nVersion;
    std::string strContractAddress;
    std::string strSenderAddress;
    std::string strReceiverAddress;
    uint256 nValue;
    uint256 transactionHash;

    // Wallet data for token transaction
    int64_t nCreateTime;
    uint256 blockHash;
    int64_t blockNumber;
    std::string strLabel;

    CTokenTx()
    {
        SetNull();
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        if (!(nType & SER_GETHASH))
        {
            //FixMe: Is thiis workround right?
            nVersion = this->nVersion;
            //READWRITE(nVersion);
            READWRITE(nCreateTime);
            READWRITE(blockHash);
            READWRITE(blockNumber);
            READWRITE(LIMITED_STRING(strLabel, 65536));
        }
        READWRITE(strContractAddress);
        READWRITE(strSenderAddress);
        READWRITE(strReceiverAddress);
        READWRITE(nValue);
        READWRITE(transactionHash);
    }

    void SetNull()
    {
        nVersion = CTokenTx::CURRENT_VERSION;
        nCreateTime = 0;
        strContractAddress = "";
        strSenderAddress = "";
        strReceiverAddress = "";
        nValue.SetNull();
        transactionHash.SetNull();
        blockHash.SetNull();
        blockNumber = -1;
        strLabel = "";
    }

    uint256 GetHash() const;
};

/** Contract book data */
class CContractBookData
{
public:
    std::string name;
    std::string abi;

    CContractBookData()
    {}
};


#endif // BITCOIN_WALLET_H

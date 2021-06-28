// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2015-2017 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_PRIMITIVES_TRANSACTION_H
#define BITCOIN_PRIMITIVES_TRANSACTION_H

#include "amount.h"
#include "script/script.h"
#include "serialize.h"
#include "uint256.h"
#include "univalue.h"
#include <list>
#include <memory>

#ifdef DPOS
typedef uint256 AgentID;
class CBitcoinAddress;
#endif

class CTransaction;
class CCoinsViewCache;

/** An outpoint - a combination of a transaction hash and an index n into its vout */
class COutPoint
{
public:
    uint256 hash;
    uint32_t n;

    COutPoint() { SetNull(); }
    COutPoint(uint256 hashIn, uint32_t nIn) { hash = hashIn; n = nIn; }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(FLATDATA(*this));
    }

    void SetNull() { hash.SetNull(); n = (uint32_t) -1; }
    bool IsNull() const { return (hash.IsNull() && n == (uint32_t) -1); }
    bool IsMasternodeReward(const CTransaction* tx) const;

    friend bool operator<(const COutPoint& a, const COutPoint& b)
    {
        return (a.hash < b.hash || (a.hash == b.hash && a.n < b.n));
    }

    friend bool operator==(const COutPoint& a, const COutPoint& b)
    {
        return (a.hash == b.hash && a.n == b.n);
    }

    friend bool operator!=(const COutPoint& a, const COutPoint& b)
    {
        return !(a == b);
    }

    std::string ToString() const;
    std::string ToStringShort() const;
	UniValue ToUniValue() const;

    uint256 GetHash();

};

/** An input of a transaction.  It contains the location of the previous
 * transaction's output that it claims and a signature that matches the
 * output's public key.
 */
class CTxIn
{
public:
    COutPoint prevout;
    CScript scriptSig;
    uint32_t nSequence;
    CScript prevPubKey;

    CTxIn()
    {
        nSequence = std::numeric_limits<unsigned int>::max();
    }

    explicit CTxIn(COutPoint prevoutIn, CScript scriptSigIn=CScript(), uint32_t nSequenceIn=std::numeric_limits<unsigned int>::max());
    CTxIn(uint256 hashPrevTx, uint32_t nOut, CScript scriptSigIn=CScript(), uint32_t nSequenceIn=std::numeric_limits<uint32_t>::max());

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(prevout);
        READWRITE(scriptSig);
        READWRITE(nSequence);
    }

    bool IsFinal() const
    {
        return (nSequence == std::numeric_limits<uint32_t>::max());
    }

    friend bool operator==(const CTxIn& a, const CTxIn& b)
    {
        return (a.prevout   == b.prevout &&
                a.scriptSig == b.scriptSig &&
                a.nSequence == b.nSequence);
    }

    friend bool operator!=(const CTxIn& a, const CTxIn& b)
    {
        return !(a == b);
    }

    std::string ToString() const;
};


/** An output of a transaction.  It contains the public key that the next input
 * must be able to sign with to claim it.
 */
class CTxOut
{
public:
    CAmount nValue;
    CScript scriptPubKey;
    int nRounds;

#ifdef DPOS
	enum class Type : uint8_t 
	{
		DEFAULT, 
		ENTRUST, 
		ENTRUST_REWARD, 
		MN_REWARD, 
		CREATE_AGENT, 
		MONTHLY_REWARD,
		GAS_REFUND,
		FUND_REWARD,
	};
	Type type;

	typedef std::string Typename;
	static Typename GetTypename(Type);
	static const Typename normalTypename;
	static const Typename entrustTypename;
	static const Typename entrustRewardTypename;
	static const Typename masternodeRewardTypename;
	static const Typename createAgentTypename;
#endif

    CTxOut()
    {
        SetNull();
    }

    CTxOut(const CAmount& nValueIn, CScript scriptPubKeyIn);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(nValue);
        READWRITE(scriptPubKey);
#ifdef DPOS
		READWRITE(*reinterpret_cast<uint8_t*>(&type));
#endif
    }

    void SetNull()
    {
        nValue = -1;
        scriptPubKey.clear();
        nRounds = -10; // an initial value, should be no way to get this by calculations
#ifdef DPOS
		type = Type::DEFAULT;
#endif
    }

    bool IsNull() const
    {
        return (nValue == -1);
    }

    void SetEmpty()
    {
        nValue = 0;
        scriptPubKey.clear();
#ifdef DPOS
		type = Type::DEFAULT;
#endif
    }

    bool IsEmpty() const
    {
        return (nValue == 0 && scriptPubKey.empty());
    }

    uint256 GetHash() const;

    bool IsDust(CFeeRate minRelayTxFee) const
    {
        // "Dust" is defined in terms of CTransaction::minRelayTxFee, which has units duffs-per-kilobyte.
        // If you'd pay more than 1/3 in fees to spend something, then we consider it dust.
        // A typical txout is 34 bytes big, and will need a CTxIn of at least 148 bytes to spend
        // i.e. total is 148 + 32 = 182 bytes. Default -minrelaytxfee is 10000 duffs per kB
        // and that means that fee per txout is 182 * 10000 / 1000 = 1820 duffs.
        // So dust is a txout less than 1820 *3 = 5460 duffs
        // with default -minrelaytxfee = minRelayTxFee = 10000 duffs per kB.
        size_t nSize = GetSerializeSize(SER_DISK,0)+148u;
        //FixMe: Is this add on correct? // gkc-vm
        return (nValue < 3*minRelayTxFee.GetFee(nSize) && !scriptPubKey.HasOpCreate() &&
                !scriptPubKey.HasOpCall() && !scriptPubKey.HasOpExtData());
    }
	
	bool IsEntrustReward()const
	{
#ifdef DPOS
		return (type == Type::ENTRUST_REWARD);
#else
		return false;
#endif
	}

	bool IsFundReward()const
	{
		return IsType(Type::FUND_REWARD);
	}
	
	bool IsMasternodeReward()const
	{
#ifdef DPOS
		return (type == Type::MN_REWARD);
#else
		return false;
#endif
	}

	bool IsGasRefund() const;

    bool IsZerocoinMint() const
    {
        return !scriptPubKey.empty() && scriptPubKey.IsZerocoinMint();
    }

    friend bool operator==(const CTxOut& a, const CTxOut& b)
    {
#ifdef DPOS
		return (a.nValue	   == b.nValue &&
				a.scriptPubKey == b.scriptPubKey &&
				a.nRounds	   == b.nRounds &&
				a.type         == b.type);
#else
		return (a.nValue	   == b.nValue &&
				a.scriptPubKey == b.scriptPubKey &&
				a.nRounds	   == b.nRounds);
#endif
    }

    friend bool operator!=(const CTxOut& a, const CTxOut& b)
    {
        return !(a == b);
    }

    std::string ToString() const;

#ifdef DPOS
	void SetType(Type);
	void SetMasternodeReward();

	bool IsEntrusted() const;
	std::vector<CBitcoinAddress> GetScriptAddresses() const;
	bool IsDeprivable() const;
	std::string GetTypeString() const;
	bool IsCreateAgentType() const;
	bool IsType(Type) const;
#endif
};

struct CMutableTransaction;

/** The basic transaction that is broadcasted on the network and contained in
 * blocks.  A transaction can contain multiple inputs and outputs.
 */
class CTransaction
{
private:
    /** Memory only. */
    const uint256 hash;
    void UpdateHash() const;

public:
    static const int32_t CURRENT_VERSION=1;

    // The local variables are made const to prevent unintended modification
    // without updating the cached hash value. However, CTransaction is not
    // actually immutable; deserialization and assignment are implemented,
    // and bypass the constness. This is safe, as they update the entire
    // structure, including the hash.
    const int32_t nVersion;
    std::vector<CTxIn> vin;
    std::vector<CTxOut> vout;
    const uint32_t nLockTime;
    //const unsigned int nTime;

#ifdef DPOS
	enum class Type 
	{ 
		DEFAULT, 
		ENTRUST, 
		DEPRIVE, 
		CREATE_AGENT, 
		RESIGN_AGENT, 
	};
	Type    type; 
	AgentID agentid; 

	typedef string Typename;
	static Typename defaultTypename;
	static Typename entrustTypename;
	static Typename depriveTypename;
	static Typename createTypename;
	static Typename resignTypename;
#endif

    /** Construct a CTransaction that qualifies as IsNull() */
    CTransaction();

    /** Convert a CMutableTransaction into a CTransaction. */
    CTransaction(const CMutableTransaction &tx);
	
    CTransaction& operator=(const CTransaction& tx);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(*const_cast<int32_t*>(&this->nVersion));
        nVersion = this->nVersion;
        READWRITE(*const_cast<std::vector<CTxIn>*>(&vin));
        READWRITE(*const_cast<std::vector<CTxOut>*>(&vout));
        READWRITE(*const_cast<uint32_t*>(&nLockTime));
		
#ifdef DPOS
        READWRITE(*const_cast<uint32_t*>((uint32_t*)&type));
        READWRITE(*const_cast<uint256*>((uint256*)&agentid));
#endif
		
        if (ser_action.ForRead())
            UpdateHash();
    }

    bool IsNull() const {
        return vin.empty() && vout.empty();
    }

    const uint256& GetHash() const {
        return hash;
    }

    // Return sum of txouts.
    CAmount GetValueOut() const;
    // GetValueIn() is a method on CCoinsViewCache, because
    // inputs must be known to compute value in.

    // Compute priority, given priority of inputs and (optionally) tx size
    double ComputePriority(double dPriorityInputs, unsigned int nTxSize=0) const;

    // Compute modified tx size for priority calculation (optionally given tx size)
    unsigned int CalculateModifiedSize(unsigned int nTxSize=0) const;

    //////////////////////////////////////// //gkc-evm
    bool HasCreateOrCall() const;

    bool HasOpCreate() const;
    bool HasOpCall() const;
    bool HasOpSpend() const;
	std::string GetContractType() const;

    bool CheckSenderScript(const CCoinsViewCache& view) const ;
    ////////////////////////////////////////

    bool IsZerocoinSpend() const
    {
        return (vin.size() > 0 && vin[0].prevout.IsNull() && vin[0].scriptSig[0] == OP_ZEROCOINSPEND);
    }

    bool IsZerocoinMint() const
    {
        for(const CTxOut& txout : vout) {
            if (txout.scriptPubKey.IsZerocoinMint())
                return true;
        }
        return false;
    }

    bool ContainsZerocoins() const
    {
        return IsZerocoinSpend() || IsZerocoinMint();
    }

    CAmount GetZerocoinMinted() const;
    CAmount GetZerocoinSpent() const;
    int GetZerocoinMintCount() const;

    bool UsesUTXO(const COutPoint out);
    std::list<COutPoint> GetOutPoints() const;

    bool IsCoinBase() const
    {
        return (vin.size() == 1 && vin[0].prevout.IsNull() && !ContainsZerocoins());
    }

    bool IsCoinBase2() const
    {
        return (vin.size() > 0 && (!vin[0].prevout.IsNull()) && vout.size() >= 2 && vout[0].IsEmpty() && 
                !vout[1].IsEmpty() && vout[1].scriptPubKey.HasOpVmHashState());
    }

    bool IsCoinStake() const
    {
        // ppcoin: the coin stake transaction is marked with the first output empty
        return (vin.size() > 0 && (!vin[0].prevout.IsNull()) && vout.size() >= 2 && vout[0].IsEmpty());
    }

    friend bool operator==(const CTransaction& a, const CTransaction& b)
    {
        return a.hash == b.hash;
    }

    friend bool operator!=(const CTransaction& a, const CTransaction& b)
    {
        return a.hash != b.hash;
    }

    std::string ToString() const;

    bool GetCoinAge(uint64_t& nCoinAge) const;  // ppcoin: get transaction coin age
    
#ifdef DPOS
	Type GetType()const;
	std::string GetTypeString() const;
	void SetType(Type);
	bool IsType(Type) const;
	bool IsEntrust()const;
	bool IsDeprive()const;
	bool IsCreateAgent()const;
	bool IsResignAgent()const;
	bool IsAdvertise()const;
	bool IsSysMessage()const;
	bool IsFilterAd()const;
	bool HasComment()const;

	//Set Type.
	void SetEntrust();
	void SetDeprive();
	void SetCreateAgent();
	void SetResignAgent();
	
	bool GetEntrustOut(CTxOut&, int& voutIndex) const;
	bool GetCreateAgentOut(CTxOut&, int& voutIndex) const;
	bool GetFirstTxOut(CTxOut::Type, CTxOut& txout, int& voutIndex) const;
	CAmount GetEntrustRewardValueOut() const;

	uint256 GetAgentId()const { return agentid; }
	void	SetAgentId(uint256 id) { agentid = id; }

	std::string GetAgentName() const;
	bool IsEntrustWithCommentTx() const;
#endif
};

/** A mutable version of CTransaction. */
struct CMutableTransaction
{
    int32_t             nVersion;
    std::vector<CTxIn>  vin;
    std::vector<CTxOut> vout;
    uint32_t            nLockTime;
#ifdef DPOS
	CTransaction::Type  type;
	AgentID             agentid;
#endif

    CMutableTransaction();
    CMutableTransaction(const CTransaction& tx);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(vin);
        READWRITE(vout);
        READWRITE(nLockTime);
#ifdef DPOS
        READWRITE(*const_cast<uint32_t*>((uint32_t*)&type));
        READWRITE(*const_cast<AgentID*>((AgentID*)&agentid));
#endif
    }

    /** Compute the hash of this CMutableTransaction. This is computed on the
     * fly, as opposed to GetHash() in CTransaction, which uses a cached result.
     */
    uint256 GetHash() const;

    std::string ToString() const;

    friend bool operator==(const CMutableTransaction& a, const CMutableTransaction& b)
    {
        return a.GetHash() == b.GetHash();
    }

    friend bool operator!=(const CMutableTransaction& a, const CMutableTransaction& b)
    {
        return !(a == b);
    }

};

#endif // BITCOIN_PRIMITIVES_TRANSACTION_H

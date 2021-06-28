// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2015-2017 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "primitives/block.h"
#include "primitives/transaction.h"

#include "chain.h"
#include "hash.h"
#include "main.h"
#include "tinyformat.h"
#include "utilstrencodings.h"
#include "transaction.h"
#include "scriptex.h"
#include <boost/foreach.hpp>

extern bool GetTransaction(const uint256 &hash, CTransaction &txOut, uint256 &hashBlock, bool fAllowSlow);

std::string COutPoint::ToString() const
{
    return strprintf("COutPoint(%s, %u)", hash.ToString(), n);
}

std::string COutPoint::ToStringShort() const
{
    return strprintf("%s-%u", hash.ToString().substr(0,64), n);
}

UniValue COutPoint::ToUniValue() const
{
	UniValue root(UniValue::VOBJ);
	root.push_back(Pair("hash",hash.ToString()));
	root.push_back(Pair("n",static_cast<int64_t>(n)));
	return root;
}

uint256 COutPoint::GetHash()
{
    return Hash(BEGIN(hash), END(hash), BEGIN(n), END(n));
}

CTxIn::CTxIn(COutPoint prevoutIn, CScript scriptSigIn, uint32_t nSequenceIn)
{
    prevout = prevoutIn;
    scriptSig = scriptSigIn;
    nSequence = nSequenceIn;
}

CTxIn::CTxIn(uint256 hashPrevTx, uint32_t nOut, CScript scriptSigIn, uint32_t nSequenceIn)
{
    prevout = COutPoint(hashPrevTx, nOut);
    scriptSig = scriptSigIn;
    nSequence = nSequenceIn;
}

std::string CTxIn::ToString() const
{
    std::string str;
    str += "CTxIn(";
    str += prevout.ToString();
    if (prevout.IsNull())
        if(scriptSig.IsZerocoinSpend())
            str += strprintf(", zerocoinspend %s", HexStr(scriptSig));
        else
            str += strprintf(", coinbase scriptSig=\"%s\"", scriptSig.ToString());
    else
        str += strprintf(", scriptSig=%s", scriptSig.ToString().substr(0,24));
    if (nSequence != std::numeric_limits<unsigned int>::max())
        str += strprintf(", nSequence=%u", nSequence);
    str += ")";
    return str;
}

CTxOut::CTxOut(const CAmount& nValueIn, CScript scriptPubKeyIn)
{
    nValue = nValueIn;
    scriptPubKey = scriptPubKeyIn;
    nRounds = -10;
#ifdef DPOS
	type = Type::DEFAULT;
#endif
}

bool COutPoint::IsMasternodeReward(const CTransaction* tx) const
{
    bool bIsMnReward;

    if(!tx->IsCoinStake())
        return false;

    if (!tx->vout[1].scriptPubKey.HasOpVmHashState())
    {
        bIsMnReward = (n == tx->vout.size() - 1) && (tx->vout[1].scriptPubKey != tx->vout[n].scriptPubKey);
    }
    else
    {
        bIsMnReward = (tx->vout.size() >= 4) && (tx->vout[2].scriptPubKey != tx->vout[n].scriptPubKey) && (tx->vout[2].scriptPubKey == tx->vout[n - 1].scriptPubKey);
    }

    return bIsMnReward;
}

uint256 CTxOut::GetHash() const
{
    return SerializeHash(*this);
}

std::string CTxOut::ToString() const
{
#ifdef DPOS
    return strprintf("CTxOut(nValue=%d.%08d, scriptPubKey=%s, type=%s)", 
    	nValue / COIN, nValue % COIN, scriptPubKey.ToString(),
    	GetTypeString());
#else
	return strprintf("CTxOut(nValue=%d.%08d, scriptPubKey=%s)", nValue / COIN, nValue % COIN, scriptPubKey.ToString());
#endif
}

#ifdef DPOS

void CTxOut::SetType(Type newType)
{
	type = newType;
}
void CTxOut::SetMasternodeReward()
{
	SetType(Type::MN_REWARD);
}
bool CTxOut::IsEntrusted() const
{
	return IsType(Type::ENTRUST);
}

std::vector<CBitcoinAddress> CTxOut::GetScriptAddresses() const
{
	std::vector<CBitcoinAddress> result;
	std::vector<CTxDestination> addresses;
    txnouttype type;
    int nRequired;
    if (!ExtractDestinations(scriptPubKey, type, addresses, nRequired))
		return result;
	for(std::vector<CTxDestination>::const_iterator it = addresses.begin(); it != addresses.end(); it++)
	{
		result.push_back(CBitcoinAddress(*it));
	}
	return result;
}
bool CTxOut::IsDeprivable() const
{
	return IsEntrusted();
}

const CTxOut::Typename CTxOut::normalTypename           = "Default";
const CTxOut::Typename CTxOut::entrustTypename          = "Entrust";
const CTxOut::Typename CTxOut::entrustRewardTypename    = "EntrustReward";
const CTxOut::Typename CTxOut::masternodeRewardTypename = "MasterNodeReward";
const CTxOut::Typename CTxOut::createAgentTypename      = "CreateAgent";

CTxOut::Typename CTxOut::GetTypename(CTxOut::Type type){
	switch(type)
	{
		case Type::DEFAULT:        return normalTypename;
		case Type::ENTRUST:        return entrustTypename;
		case Type::ENTRUST_REWARD: return entrustRewardTypename;
		case Type::MN_REWARD:      return masternodeRewardTypename;
		case Type::CREATE_AGENT:   return createAgentTypename;
		case Type::MONTHLY_REWARD:   return "MonthlyReward";
		case Type::GAS_REFUND:     return "GasRefund";
		case Type::FUND_REWARD:    return "FundReward";
	}
	assert(0);
	throw std::logic_error("Undefined txout type");
	return "Undefined";
}

std::string CTxOut::GetTypeString() const
{
	return GetTypename(type);
}
bool CTxOut::IsCreateAgentType() const
{
	return IsType(Type::CREATE_AGENT);
}
bool CTxOut::IsType(Type a) const
{
	return type == a;
}
#endif

bool CTxOut::IsGasRefund() const
{
	return IsType(Type::GAS_REFUND);
}

CMutableTransaction::CMutableTransaction() : nVersion(CTransaction::CURRENT_VERSION), nLockTime(0) {
#ifdef DPOS
	type = CTransaction::Type::DEFAULT;
	agentid = 0;
#endif
}
CMutableTransaction::CMutableTransaction(const CTransaction& tx) : nVersion(tx.nVersion), vin(tx.vin), vout(tx.vout), nLockTime(tx.nLockTime) {
#ifdef DPOS
	type = tx.type;
	agentid = tx.agentid;
#endif
}

uint256 CMutableTransaction::GetHash() const
{
    return SerializeHash(*this);
}

std::string CMutableTransaction::ToString() const
{
    std::string str;
#ifdef DPOS
	str += strprintf("CMutableTransaction(ver=%d, vin.size=%u, vout.size=%u, nLockTime=%u, type=%d, agentid=%s)\n",
		nVersion,
		vin.size(),
		vout.size(),
		nLockTime,
		(int)type,
		agentid.GetHex().c_str());
#else
    str += strprintf("CMutableTransaction(ver=%d, vin.size=%u, vout.size=%u, nLockTime=%u)\n",
        nVersion,
        vin.size(),
        vout.size(),
        nLockTime);
#endif
    for (unsigned int i = 0; i < vin.size(); i++)
        str += "    " + vin[i].ToString() + "\n";
    for (unsigned int i = 0; i < vout.size(); i++)
        str += "    " + vout[i].ToString() + "\n";
    return str;
}

void CTransaction::UpdateHash() const
{
    *const_cast<uint256*>(&hash) = SerializeHash(*this);
}

#ifdef DPOS
string CTransaction::defaultTypename = "Default";
string CTransaction::entrustTypename = "Entrust";
string CTransaction::depriveTypename = "Deprive";
string CTransaction::createTypename = "CreateAgent";
string CTransaction::resignTypename = "ResignAgent";
#endif

CTransaction::CTransaction() : hash(), nVersion(CTransaction::CURRENT_VERSION), vin(), vout(), nLockTime(0) { 
#ifdef DPOS
	type = Type::DEFAULT;
	agentid = 0;
#endif
}

CTransaction::CTransaction(const CMutableTransaction &tx) : nVersion(tx.nVersion), vin(tx.vin), vout(tx.vout), nLockTime(tx.nLockTime) {
#ifdef DPOS
	type = tx.type;
	agentid = tx.agentid;
#endif
    UpdateHash();
}

CTransaction& CTransaction::operator=(const CTransaction &tx) {
    *const_cast<int*>(&nVersion) = tx.nVersion;
    *const_cast<std::vector<CTxIn>*>(&vin) = tx.vin;
    *const_cast<std::vector<CTxOut>*>(&vout) = tx.vout;
    *const_cast<unsigned int*>(&nLockTime) = tx.nLockTime;
    *const_cast<uint256*>(&hash) = tx.hash;
	
#ifdef DPOS
    *const_cast<Type*>(&type) = tx.type;
	*const_cast<AgentID*>(&agentid) = tx.agentid;
#endif

    return *this;
}

CAmount CTransaction::GetValueOut() const
{
    CAmount nValueOut = 0;
    for (std::vector<CTxOut>::const_iterator it(vout.begin()); it != vout.end(); ++it)
    {
        // GKC: previously MoneyRange() was called here. This has been replaced with negative check and boundary wrap check.
        if (it->nValue < 0)
            throw std::runtime_error("CTransaction::GetValueOut() : value out of range : less than 0");

        if ((nValueOut + it->nValue) < nValueOut)
            throw std::runtime_error("CTransaction::GetValueOut() : value out of range : wraps the int64_t boundary");

        nValueOut += it->nValue;
    }
    return nValueOut;
}

CAmount CTransaction::GetZerocoinMinted() const
{
    for (const CTxOut txOut : vout) {
        if(!txOut.scriptPubKey.IsZerocoinMint())
            continue;

        return txOut.nValue;
    }

    return  CAmount(0);
}

bool CTransaction::UsesUTXO(const COutPoint out)
{
    for (const CTxIn in : vin) {
        if (in.prevout == out)
            return true;
    }

    return false;
}

std::list<COutPoint> CTransaction::GetOutPoints() const
{
    std::list<COutPoint> listOutPoints;
    uint256 txHash = GetHash();
    for (unsigned int i = 0; i < vout.size(); i++)
        listOutPoints.emplace_back(COutPoint(txHash, i));
    return listOutPoints;
}

CAmount CTransaction::GetZerocoinSpent() const
{
    if(!IsZerocoinSpend())
        return 0;

    CAmount nValueOut = 0;
    for (const CTxIn txin : vin) {
        if(!txin.scriptSig.IsZerocoinSpend())
            LogPrintf("%s is not zcspend\n", __func__);

        std::vector<char, zero_after_free_allocator<char> > dataTxIn;
        dataTxIn.insert(dataTxIn.end(), txin.scriptSig.begin() + 4, txin.scriptSig.end());

        CDataStream serializedCoinSpend(dataTxIn, SER_NETWORK, PROTOCOL_VERSION);
        libzerocoin::CoinSpend spend(Params().Zerocoin_Params(), serializedCoinSpend);
        nValueOut += libzerocoin::ZerocoinDenominationToAmount(spend.getDenomination());
    }

    return nValueOut;
}

int CTransaction::GetZerocoinMintCount() const
{
    int nCount = 0;
    for (const CTxOut out : vout) {
        if (out.scriptPubKey.IsZerocoinMint())
            nCount++;
    }
    return nCount;
}

double CTransaction::ComputePriority(double dPriorityInputs, unsigned int nTxSize) const
{
    nTxSize = CalculateModifiedSize(nTxSize);
    if (nTxSize == 0) return 0.0;

    return dPriorityInputs / nTxSize;
}

unsigned int CTransaction::CalculateModifiedSize(unsigned int nTxSize) const
{
    // In order to avoid disincentivizing cleaning up the UTXO set we don't count
    // the constant overhead for each txin and up to 110 bytes of scriptSig (which
    // is enough to cover a compressed pubkey p2sh redemption) for priority.
    // Providing any more cleanup incentive than making additional inputs free would
    // risk encouraging people to create junk outputs to redeem later.
    if (nTxSize == 0)
        nTxSize = ::GetSerializeSize(*this, SER_NETWORK, PROTOCOL_VERSION);
    for (std::vector<CTxIn>::const_iterator it(vin.begin()); it != vin.end(); ++it)
    {
        unsigned int offset = 41U + std::min(110U, (unsigned int)it->scriptSig.size());
        if (nTxSize > offset)
            nTxSize -= offset;
    }
    return nTxSize;
}

///////////////////////////////////////////////////////////// //gkc-vm
bool CTransaction::HasCreateOrCall() const
{
    //  FixMe:  IsEnabled means the contract enabled or not.

//    bool IsEnabled =  chainActive.Tip()->IsContractEnabled();

//    if (!IsEnabled)
//    {
//        return false;
//    }

    for (const CTxOut &v : vout)
    {
        if (v.scriptPubKey.HasOpCreate() || v.scriptPubKey.HasOpCall())
        {
            return true;
        }
    }
    return false;
}

bool CTransaction::HasOpCreate() const
{
    for (const CTxOut &v : vout)
    {
        if (v.scriptPubKey.HasOpCreate())
        {
            return true;
        }
    }
}

bool CTransaction::HasOpCall() const
{
    for (const CTxOut &v : vout)
    {
        if (v.scriptPubKey.HasOpCall())
        {
            return true;
        }
    }
}

bool CTransaction::HasOpSpend() const
{
//    bool IsEnabled =  chainActive.Tip()->IsContractEnabled();

//    if (!IsEnabled)
//    {
//        return false;
//    }

    for (const CTxIn &i : vin)
    {
        if (i.scriptSig.HasOpSpend())
        {
            return true;
        }
    }
    return false;
}

std::string CTransaction::GetContractType() const
{
	if(HasOpCreate())
		return "Create";
	else if(HasOpCall())
		return "Call";
	else if(HasOpSpend())
		return "Spend";
	else
		return "";
	assert(0);
	return "";
}

/////////////////////////////////////////////////////////////gkc-vm

bool CTransaction::CheckSenderScript(const CCoinsViewCache &view) const
{


//    bool IsEnabled =  chainActive.Tip()->IsContractEnabled();

//    if (!IsEnabled)
//    {
//        return false;
//    }

    //FixMe: Is this correct:vout[vin[0].prevout.n]?

    CScript script = view.AccessCoins(vin[0].prevout.hash)->vout[vin[0].prevout.n].scriptPubKey;
    // CScript script = view.AccessCoin(vin[0].prevout).out.scriptPubKey;

    if (!script.IsPayToPubkeyHash() && !script.IsPayToPubkey())
    {
        return false;
    }
    return true;
}
/////////////////////////////////////////////////////////////

#ifdef DPOS
CTransaction::Type CTransaction::GetType()const
{
	return type;
}
std::string CTransaction::GetTypeString() const
{
	std::string contractType = GetContractType();
    if(!contractType.empty())
		return contractType;
	switch(type)
	{
		case Type::ENTRUST: return entrustTypename;
		case Type::DEPRIVE: return depriveTypename;
		case Type::DEFAULT: return defaultTypename;
		case Type::CREATE_AGENT: return createTypename;
		case Type::RESIGN_AGENT: return resignTypename;
	}
	throw std::logic_error("Undefined tx type");
	return "Undefined";
}
void CTransaction::SetType(Type t)
{
	type = t;
}
bool CTransaction::IsType(Type t) const
{
	return type == t;
}
bool CTransaction::IsEntrust()const
{ 
	return IsType(Type::ENTRUST); 
}
bool CTransaction::IsDeprive()const 
{ 
	return IsType(Type::DEPRIVE); 
}
bool CTransaction::IsCreateAgent()const
{
	return IsType(Type::CREATE_AGENT); 
}
bool CTransaction::IsResignAgent()const
{
	return IsType(Type::RESIGN_AGENT);
}
bool CTransaction::IsAdvertise()const
{
	BOOST_FOREACH (const CTxOut& txout, this->vout) {
		ScriptEx scriptex(txout.scriptPubKey);
		if(scriptex.IsAdvertise())
			return true;
	}
	return false;
}
bool CTransaction::IsSysMessage()const
{
	BOOST_FOREACH (const CTxOut& txout, this->vout) {
		ScriptEx scriptex(txout.scriptPubKey);
		if(scriptex.IsSysMessage())
			return true;
	}
	return false;
}
bool CTransaction::IsFilterAd()const
{
	BOOST_FOREACH (const CTxOut& txout, this->vout) {
		ScriptEx scriptex(txout.scriptPubKey);
		if(scriptex.IsFilterAd())
			return true;
	}
	return false;
}
bool CTransaction::HasComment()const
{
	for (const CTxOut& txout: this->vout) {
		ScriptEx scriptex(txout.scriptPubKey);
		if(scriptex.IsComment()) {
			return true;
		}
	}
	return false;
}

void CTransaction::SetEntrust()
{ 
	SetType(Type::ENTRUST); 
}
void CTransaction::SetDeprive()
{ 
	SetType(Type::DEPRIVE);
}
void CTransaction::SetCreateAgent()
{ 
	SetType(Type::CREATE_AGENT);
}
void CTransaction::SetResignAgent()
{
	SetType(Type::RESIGN_AGENT);
}
bool CTransaction::GetEntrustOut(CTxOut& txout, int& voutIndex) const
{
	return GetFirstTxOut(CTxOut::Type::ENTRUST,txout,voutIndex);
}
bool CTransaction::GetCreateAgentOut(CTxOut& txout, int& voutIndex) const
{
	return GetFirstTxOut(CTxOut::Type::CREATE_AGENT,txout,voutIndex);
}
bool CTransaction::GetFirstTxOut(CTxOut::Type txoutType, CTxOut& txout, int& voutIndex) const
{
	for(int i=0; i<vout.size(); i++)
	{
		if (vout[i].IsType(txoutType))
		{
			txout = vout[i];
			voutIndex = i;
			return true;
		}
	}
	return false;
}
CAmount CTransaction::GetEntrustRewardValueOut() const
{
	CAmount total = 0;
	for(int i=0; i<vout.size(); i++){
		if (vout[i].IsEntrustReward()){
			total += vout[i].nValue;
		}
	}
	return total;
}
std::string CTransaction::GetAgentName() const {
	std::string name;
	if(!IsCreateAgent())
		return name;
	for(int i=0; i<vout.size(); i++){
		const CTxOut& txout = vout[i];
		if (txout.nValue==0 && txout.scriptPubKey.HasOpReturn()) {
			ScriptEx scriptex(txout.scriptPubKey);
			if(scriptex.IsComment())
			{
				advertisement::TxComment comment;
				comment.DecodeFrom(txout.scriptPubKey);
				name = comment.content;
				break;
			}
			// OP_RETURN (SPACE) DPOSNAME
			name = std::string(txout.scriptPubKey.begin()+2,txout.scriptPubKey.end());
			break;
		}
	}
	return name;
}

#endif

std::string CTransaction::ToString() const
{
    std::string str;
#ifdef DPOS
    str += strprintf("CTransaction(hash=%s, ver=%d, vin.size=%u, vout.size=%u, nLockTime=%u, type=%s, agentid=%s)\n",
        GetHash().ToString(),
        nVersion,
        vin.size(),
        vout.size(),
        nLockTime,
        GetTypeString(),
        agentid.GetHex().c_str());
#else
    str += strprintf("CTransaction(hash=%s, ver=%d, vin.size=%u, vout.size=%u, nLockTime=%u)\n",
        GetHash().ToString(),
        nVersion,
        vin.size(),
        vout.size(),
        nLockTime);
#endif
    for (unsigned int i = 0; i < vin.size(); i++)
        str += "    " + vin[i].ToString() + "\n";
    for (unsigned int i = 0; i < vout.size(); i++)
        str += "    " + vout[i].ToString() + "\n";
    return str;
}

bool CTransaction::IsEntrustWithCommentTx() const
{
	return IsEntrust() && HasComment();
}


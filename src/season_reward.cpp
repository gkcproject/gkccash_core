
#include "season_reward.h"

#include "util.h" // util::HexStringToBytes
#include "scriptex.h" // scriptex

using namespace seasonreward;

SeasonRewardAdmin& SeasonRewardAdmin::GetInstance()
{
	static SeasonRewardAdmin singleton;
	return singleton;
}

SeasonRewardAdmin::SeasonRewardAdmin()
{
	bytes_t pubKeyData = util::HexStringToBytes("0300230a5d8ce15117eb685c292f2e42b2d733b0b64e549199c89e3dca925b8ced");
	pubkey = CPubKey(pubKeyData);
}

CPubKey SeasonRewardAdmin::GetPubKey() const
{
	return pubkey;
}

uint256 seasonreward::HashBlackAgentTx(const std::vector<CTxIn>& inputs, const AgentID& agentid)
{
    CHashWriter ss(SER_GETHASH, 0);
	for(const auto& txin : inputs)
		ss << txin.prevout;
	ss << agentid;
    return ss.GetHash();
}

bytes_t seasonreward::AdminSign(const uint256& src, const CKeyStore& keyStore)
{
	bytes_t sig;
	
	CKey privkey;
	if(!keyStore.GetKey(SeasonRewardAdmin::GetInstance().GetPubKey().GetID(),privkey))
		throw std::runtime_error("privkey not found");
		
	if(!privkey.Sign(src,sig))
		throw std::runtime_error("privkey.Sign() faield");
	
	return sig;
}

CScript seasonreward::AppendSigToBlackAgentScript(const CScript& src, const bytes_t& signature)
{
	CScript script = src;
	script << signature;
	return script;
}

bool seasonreward::VerifyAddBlackAgentTx(const std::vector<CTxIn>& inputs, const CScript& script)
{
	AgentID agentid;
	bytes_t signature;
	scriptex::DecodeBlackAgentFromScript(script,agentid,signature);
	uint256 signsrc = seasonreward::HashBlackAgentTx(inputs,agentid);
	CPubKey pubKey = SeasonRewardAdmin::GetInstance().GetPubKey();
	return pubKey.Verify(signsrc,signature);
}



#ifndef SEASON_REWARD_H
#define SEASON_REWARD_H

// header // symbols
#include "pubkey.h" // CPubKey
#include "uint256.h" // uint256
#include <vector> // std::vector
#include "primitives/transaction.h" // CTxIn, AgentID
#include "script/script.h" // CScript
#include "keystore.h" // CKeyStore

namespace seasonreward
{
	typedef std::vector<unsigned char> bytes_t;

	class SeasonRewardAdmin
	{
	public:	
		static SeasonRewardAdmin& GetInstance();
		CPubKey GetPubKey() const;
	protected:
		SeasonRewardAdmin();
	private:
		CPubKey pubkey;
	};

	uint256 HashBlackAgentTx(const std::vector<CTxIn>& txvin, const AgentID&);

	bytes_t AdminSign(const uint256& src, const CKeyStore&);

	CScript AppendSigToBlackAgentScript(const CScript& src, const bytes_t& signature);

	bool VerifyAddBlackAgentTx(const std::vector<CTxIn>& txvin, const CScript&);
}

#endif // SEASON_REWARD_H

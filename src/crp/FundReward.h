#ifndef CRP_FUNDREWARD_H
#define CRP_FUNDREWARD_H

#include "CoinReleasePlan.h"
#include "base58.h" // CBitcoinAddress
#include "primitives/transaction.h" // CTxOut
#include <vector> // std::vector
#include "amount.h" // CAmount
#include <utility> // std::pair

namespace fundreward
{
	CBitcoinAddress GetFundRewardAddress(BlockHeight blockheight);
	
	void AppendFundReward(const CBitcoinAddress& addr, CAmount amount, std::vector<CTxOut>& vout);

	void AppendRewardTxout(CTxOut::Type type, const std::vector<std::pair<CBitcoinAddress,CAmount> >& rewardList, std::vector<CTxOut>& vout);

	bool CheckBlock(const CBlock&);
	bool CheckCoinStake(BlockHeight, const CTransaction&);
}//~namespace


#endif //~CRP_FUNDREWARD_H

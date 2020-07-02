
#include "FundReward.h"

#include <string>
#include "main.h" // chainActive

using namespace fundreward;
using std::string;

namespace
{
	const char* LOGLABEL = "fundreward";
	const char* SPACENAME = "fundreward";
}

CBitcoinAddress fundreward::GetFundRewardAddress(BlockHeight blockheight)
{
	return CBitcoinAddress("GJKkwwNM7hsgbg4p6pKKaStVDBmYQrBS87"); // address of Season Reward Admin wallet, bip44-path: m/44'/0'/0'/0/1
}

void fundreward::AppendFundReward(const CBitcoinAddress& addr, CAmount amount, std::vector<CTxOut>& vout)
{
	LogPrint(LOGLABEL,"%s::%s|INFO|addr=%s,amount=%lld\n",SPACENAME,__func__,addr.ToString(),amount);
	std::vector<std::pair<CBitcoinAddress,CAmount> > fundRewardLis;
	std::pair<CBitcoinAddress,CAmount> kv(addr,amount);
	fundRewardLis.push_back(kv);
	AppendRewardTxout(CTxOut::Type::FUND_REWARD,fundRewardLis,vout);
}

void fundreward::AppendRewardTxout(CTxOut::Type type, const std::vector<std::pair<CBitcoinAddress,CAmount> >& rewardList, std::vector<CTxOut>& vout)
{
	LogPrint(LOGLABEL,"%s::%s|INFO|type=%d,rewardList.size()=%u\n",SPACENAME,__func__,(int)type,rewardList.size());
	for(std::vector<std::pair<CBitcoinAddress,CAmount> >::const_iterator it = rewardList.begin(); it != rewardList.end(); it++)
	{
		const CBitcoinAddress& addr = it->first;
		const CAmount& reward = it->second;
		CTxOut txout(reward,GetScriptForDestination(addr.Get()));
		txout.SetType(type);
		LogPrint(LOGLABEL,"%s::%s|INFO|addr=%s, type=%d, txout=%s\n",SPACENAME,__func__,addr.ToString(),(int)type,txout.ToString());
		vout.push_back(txout);
	}
}

bool fundreward::CheckBlock(const CBlock& block)
{
	LogPrint(LOGLABEL,"%s::%s|INFO|blockhash=%s\n",SPACENAME,__func__,block.GetHash().ToString());
    const CBlockIndex* pindexPrev = chainActive.Tip();
	if(!pindexPrev)
	{
		LogPrintf("%s::%s|WARN|pindexPrev is null, blockhash=%s\n",SPACENAME,__func__,block.GetHash().ToString());
		return true;
	}
	
    int blockheight = pindexPrev->GetBlockHash() == block.hashPrevBlock ? pindexPrev->nHeight + 1 : -1;
	if(blockheight < 0)
	{
		LogPrint(LOGLABEL,"%s::%s|INFO|blockheight not found, blockhash=%s\n",SPACENAME,__func__,block.GetHash().ToString());
		return true;
	}

	const CTransaction* coinstake = block.GetCoinStake();
	if(!coinstake)
	{
		LogPrintf("%s::%s|ERROR|coinstake not found, blockheight=%d, blockhash=%s\n",SPACENAME,__func__,blockheight,block.GetHash().ToString());
		return true;
	}

	return fundreward::CheckCoinStake(blockheight,*coinstake);
}

bool fundreward::CheckCoinStake(BlockHeight blockheight, const CTransaction& coinstake)
{
	LogPrint(LOGLABEL,"%s::%s|INFO|blockheight=%d, coinstake=%s\n",SPACENAME,__func__,blockheight,coinstake.GetHash().ToString());
	CAmount actual = 0;
	size_t voutsize = coinstake.vout.size();
	for(int i=0; i<voutsize; i++)
	{
		const CTxOut& txout = coinstake.vout[i];
		if(txout.type == CTxOut::Type::FUND_REWARD)
		{
			actual += txout.nValue;
			LogPrint(LOGLABEL,"%s::%s|INFO|vout=%d, nValue=%lld, actual=%lld\n",SPACENAME,__func__,i,txout.nValue,actual);
			std::vector<CBitcoinAddress> addresses = txout.GetScriptAddresses();
			if(addresses.empty())
			{
				LogPrintf("%s::%s|WARN|address in script is empty, blockheight=%d, coinstake=%s, vout=%d, txout=%s\n",SPACENAME,__func__,blockheight,coinstake.GetHash().ToString(),i,txout.ToString());
				return false;
			}
			CBitcoinAddress user = addresses[0];
			CBitcoinAddress fundAddress = fundreward::GetFundRewardAddress(blockheight);
			LogPrint(LOGLABEL,"%s::%s|INFO|vout=%d,user=%s,fundAddress=%s\n",SPACENAME,__func__,i,user.ToString(),fundAddress.ToString());
			if(user != fundAddress)
			{
				LogPrintf("%s::%s|WARN|user is not a valid fund address, blockheight=%d, coinstake=%s, vout=%d, user=%s, fundAddress=%s\n",SPACENAME,__func__,blockheight,coinstake.GetHash().ToString(),i,user.ToString(),fundAddress.ToString());
				return false;
			}
		}		
	}

	CAmount expected = crp::CoinReleasePlan::GetInstance().GetBlockValue(blockheight).fund;
	LogPrint(LOGLABEL,"%s::%s|INFO|actual=%lld, expected=%lld\n",SPACENAME,__func__,actual,expected);
	if(actual != expected)
	{
		LogPrintf("%s::%s|WARN|The actual foundation reward does not match the expected value, blockheight=%d, coinstake=%s, actual=%lld, expected=%lld\n",SPACENAME,__func__,blockheight,coinstake.GetHash().ToString(),actual,expected);
		return false;
	}
	
	return true;
}


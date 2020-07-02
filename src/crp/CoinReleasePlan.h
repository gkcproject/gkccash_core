#ifndef crp_CoinReleasePlan_h
#define crp_CoinReleasePlan_h

#include <string>
#include <ctime>
#include <vector>
#include <cstdint>
#include <cassert>
#include "uint256.h"

namespace crp // coin_release_plan
{
	typedef int Height;
	typedef int64_t CoinAmount;
	typedef Height BlockAmount;
	typedef int Agentid;
	typedef uint256 WeightNumber;

	extern const CoinAmount COIN;
	extern const CoinAmount WAN;
	extern const time_t MONTH;
	extern const time_t DAY;
	extern const time_t HOUR;
	extern const time_t MINUTE;
	extern const time_t SECOND;
	
	class BlockValue
	{
	public:
		BlockValue();
		CoinAmount miner;
		CoinAmount fund;
		CoinAmount entrust;
		CoinAmount masternode;
		CoinAmount topFirst;
		CoinAmount topSecond;
		CoinAmount Total() const;
		std::string ToString() const;
	};
	bool operator== (const BlockValue& left, const BlockValue& right);
	bool operator!= (const BlockValue& left, const BlockValue& right);

	class HeightRange{
	public:
		HeightRange();
		Height Begin() const;
		Height Back() const;
		Height End() const;
		bool Exists(Height) const;
		static Height InfiniteHeight();
	public:
		Height begin;
		Height range;
	};

	/*
	get start/last point of range for the x:
	
	start0 = 2
	range  = 3
	x	   = 7
	start  = ?
	
	x:     0 1 2 3 4 5 6 7 8 9 0 ...
	range:     = = = - - - = = = ...
				         ^
	start  = 5
	last   = 7
	*/
	template <typename Num>
	int GetStartOfRange(Num start0, Num range, Num x)
	{
		assert(range > 0);
	
		const Num offset = x - start0;
		if(offset < 0)
			return start0;
	
		return start0 + Num(offset / range) * range;
	}
	
	template <typename Num>
	int GetLastOfRange(Num start0, Num range, Num x)
	{
		return GetStartOfRange(start0,range,x) + range - 1;
	}
	
	class Plan{
	public:
		time_t velocity; // time unit in second
		HeightRange heightRange;
	public:
		BlockAmount BlockNumPerMonth() const;
	};

	class PowPlan : public Plan{
	public:
		CoinAmount firstBlockRewardAmount;
		BlockAmount stableBlockNum;
		CoinAmount stableBlockReward;
	};

	class PosPlan : public Plan{
	public:
		CoinAmount standardMinerReward;
		CoinAmount standardFundReward;
		CoinAmount standardEntrustReward;
		CoinAmount standardMasternodeReward;
		CoinAmount monthlyTopFirst;
		CoinAmount monthlyTopSecond;
		CoinAmount boobyPrize; //Rewards for nodes that are on the leaderboard but have not made a block
	};
		
	class Agent {
	public:
		Agent();
		Agentid id;
		BlockAmount blockAmount;
		WeightNumber weight;
		time_t lastBlockTime;
	};

	class Season {
	public:
		int    id;
		Height first_height,
			   last_height,
			   reward_height;
		Season(){
			id = -1;
			first_height = last_height = reward_height = -1;
		}
	};
		
	class CoinReleasePlan
	{
	public:
		static CoinReleasePlan& GetInstance();
		CoinReleasePlan();
		PowPlan pow;
		PosPlan pos_v1;
		PosPlan pos_v2;
		Height seasonIntervalHeight;
		
		Height forkHeightForSeasonRewardV2;
		Height forkheight_seasonIntervalHeight;
		Height forkheight_posV2;

		Height GetSeasonIntervalHeight(Height chainHeight) const;
		BlockValue GetBlockValue(Height) const;
		bool IsStartHeight(Height) const;
		bool InPowHeightRange(Height) const;
		bool InPosHeightRange(Height) const;
		bool IsSeasonRewardHeight(Height) const;
		bool IsSeasonRewardV2(Height) const;
		Season GetSeason(Height) const;
		const PosPlan& GetPosPlan(Height) const;
	private:
		bool IsPosV2(Height) const;
	};

	class DistributeRule
	{
	public:
		int topFirstCount;
		int topSecondCount;
		DistributeRule(int first, int second):
			topFirstCount(first),topSecondCount(second){}
		int TotalCount() const;
	};

	std::vector<std::pair<Agentid,CoinAmount>> GetMonthlyDistribution(const std::vector<Agent>& blockCountVec, CoinAmount topFirstTotalAmount, CoinAmount topSecondTotalAmount, const DistributeRule& rule, CoinAmount boobyPrize);
	bool LeftAgentMoreActive(const Agent& left, const Agent& right);
	bool IsPowHeight(Height);
	bool IsPosHeight(Height);
	BlockValue GetBlockValue(Height);
	Height GetPosStartHeight();
	
	int64_t WeightAmount(int64_t totalAmount, uint256 weight, uint256 totalWeight);
}//~namespace


#endif

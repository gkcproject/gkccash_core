#include "CoinReleasePlan.h"
#include <cassert>
#include <sstream>
#include <cmath>
#include <limits>
#include "uint256.h"
#include "main.h"
#include "forkheights.h" // forkheight_release_v_2_5_1

using namespace std;
using namespace crp;

const CoinAmount crp::COIN = 100000000;
const CoinAmount crp::WAN = 10000*COIN;

const time_t crp::SECOND = 1;
const time_t crp::MINUTE = 60*SECOND;
const time_t crp::HOUR = 60*MINUTE;
const time_t crp::DAY = 24*HOUR;
const time_t crp::MONTH = 31*DAY;

BlockValue::BlockValue(){
	miner = 0;
	fund = 0;
	entrust = 0;
	masternode = 0;
	topFirst = 0;
	topSecond = 0;
}
HeightRange::HeightRange(){
	begin = 0;
	range = InfiniteHeight();
}
Height HeightRange::Begin() const{
	return begin;
}
Height HeightRange::Back() const{
	if(End() == InfiniteHeight())
		return InfiniteHeight();
	return End() - 1;
}
Height HeightRange::End() const{
	if(range == InfiniteHeight())
		return InfiniteHeight();
	return begin + range;
}
bool HeightRange::Exists(Height h) const{
	if(range == InfiniteHeight())
		return h >= Begin();
	assert(Back() != InfiniteHeight());
	return Begin() <= h && h <= Back();
}
Height HeightRange::InfiniteHeight(){
	return std::numeric_limits<Height>::max();
}

crp::Agent::Agent(){
	id = 0;
	blockAmount = 0;
	weight = 0;
	lastBlockTime = 0;
}
CoinReleasePlan::CoinReleasePlan(){
	pow.firstBlockRewardAmount = 2300*WAN;
	pow.velocity               = 1*MINUTE;
	pow.stableBlockNum         = 2999;
  	pow.stableBlockReward      = 170*COIN;
  	pow.heightRange.begin      = 1;
  	pow.heightRange.range      = 1 + pow.stableBlockNum;

	pos_v1.velocity                 = 1*MINUTE;
	pos_v1.standardMinerReward      =   1.2 * COIN;
	pos_v1.standardFundReward       =   0;
	pos_v1.standardEntrustReward    =   6.6 * COIN;
	pos_v1.standardMasternodeReward =   0.6 * COIN;
	pos_v1.monthlyTopFirst          = 78000 * COIN;
	pos_v1.monthlyTopSecond         = 78000 * COIN;
	pos_v1.heightRange.begin        = pow.heightRange.End();
	pos_v1.heightRange.range        = HeightRange::InfiniteHeight();
	pos_v1.boobyPrize               = 10 * COIN; //a prize that is given to the person who is not generates POS block in a monthly competition

	pos_v2 = pos_v1;
	pos_v2.standardFundReward       =  0.35 * COIN;
	pos_v2.standardMasternodeReward =  0.48 * COIN;
	pos_v2.monthlyTopFirst          = 72800 * COIN;
	pos_v2.monthlyTopSecond         = 72800 * COIN;
	
	forkHeightForSeasonRewardV2  = 92281; // second reward height of season
	seasonIntervalHeight         = pos_v1.BlockNumPerMonth();
	forkheight_seasonIntervalHeight = std::numeric_limits<Height>::max(); // never fork
	forkheight_posV2 = forkheight_release_v_2_5_1;
}

Height crp::CoinReleasePlan::GetSeasonIntervalHeight(Height chainHeight) const
{
	Height result = GetPosPlan(chainHeight).BlockNumPerMonth();
	if(chainHeight >= forkheight_seasonIntervalHeight)
		result = seasonIntervalHeight;
	return result;
}

BlockAmount Plan::BlockNumPerMonth() const{
	return MONTH / velocity;
}

BlockValue CoinReleasePlan::GetBlockValue(Height h) const{
	BlockValue v;
	if(IsStartHeight(h)){
		v.miner = pow.firstBlockRewardAmount;
	}
	else if(InPowHeightRange(h)){
		v.miner = pow.stableBlockReward;
	}
	else if(InPosHeightRange(h)){
		const PosPlan& posplan = GetPosPlan(h);
		Height start = posplan.heightRange.Begin();
		Height interval = 8*posplan.BlockNumPerMonth();
		v.miner = halve(posplan.standardMinerReward,start,interval,h);
		v.fund = halve(posplan.standardFundReward,start,interval,h);
		v.entrust = halve(posplan.standardEntrustReward,start,interval,h);
		v.masternode = halve(posplan.standardMasternodeReward,start,interval,h);
		if(IsSeasonRewardHeight(h)){
			v.topFirst = halve(posplan.monthlyTopFirst,start,interval,h);
			v.topSecond = halve(posplan.monthlyTopSecond,start,interval,h);
		}
	}
	else{
		//undefined
	}
	return v;
}
const PosPlan& CoinReleasePlan::GetPosPlan(Height h) const{
	if(IsPosV2(h))
		return pos_v2;
	return pos_v1;
}
bool CoinReleasePlan::IsPosV2(Height h) const{
	return h >= forkheight_posV2;
}
bool CoinReleasePlan::IsSeasonRewardV2(Height h) const {
	return h >= forkHeightForSeasonRewardV2;
}
bool CoinReleasePlan::IsStartHeight(Height h) const{
	return h == pow.heightRange.Begin();
}
bool CoinReleasePlan::InPowHeightRange(Height h) const{
	return pow.heightRange.Exists(h);
}
bool CoinReleasePlan::InPosHeightRange(Height h) const{
	return GetPosPlan(h).heightRange.Exists(h);
}
bool CoinReleasePlan::IsSeasonRewardHeight(Height h) const{
	const Height first = GetPosPlan(h).heightRange.Begin();
	const BlockAmount range = GetSeasonIntervalHeight(h);
	const Height fork = first + range;

	if(h < first)
		return false;
	else if(h < fork)
		return (h-(first-1)) % range == 0;
	else if(h == fork)
		return false;
	else
		return (h - first) % range == 0;

	assert(0);
	return false;
}
Season CoinReleasePlan::GetSeason(Height h) const{
	Season season;
	const Height first = GetPosPlan(h).heightRange.Begin();
	const BlockAmount range = GetSeasonIntervalHeight(h);
	const Height season_offset = h - first;
	season.id = int(season_offset/range);
	season.first_height = first + season.id * range;
	season.reward_height = season.first_height + range;
	season.last_height = season.reward_height - 1;
	if(season.id == 0)
		season.reward_height = season.last_height;
	assert(IsSeasonRewardHeight(season.reward_height));
	return season;
}

int crp::DistributeRule::TotalCount() const
{
	return topFirstCount + topSecondCount;
}

std::string BlockValue::ToString() const{
	std::ostringstream os;
	os << "{"
		<< "\"miner\":" << miner << ","
		<< "\"fund\":" << fund << ","
		<< "\"entrust\":" << entrust << ","
		<< "\"masternode\":" << masternode << ","
		<< "\"topFirst\":" << topFirst << ","
		<< "\"topSecond\":" << topSecond << ","
		<< "\"Total\":" << Total()
		<< "}";
	return os.str();
}
CoinAmount BlockValue::Total() const{
	return miner + fund + entrust + masternode + topFirst + topSecond;
}
bool crp::operator== (const BlockValue& left, const BlockValue& right){
	return left.miner == right.miner
		&& left.fund == right.fund
		&& left.entrust == right.entrust
		&& left.masternode == right.masternode
		&& left.topFirst == right.topFirst
		&& left.topSecond == right.topSecond
		&& left.Total() == right.Total();
}
bool crp::operator!= (const BlockValue& left, const BlockValue& right) {
	return !(left==right);
}
std::vector<std::pair<Agentid,CoinAmount>> crp::GetMonthlyDistribution(const std::vector<Agent>& agentVec, CoinAmount topFirstTotalAmount, CoinAmount topSecondTotalAmount, const DistributeRule& rule, CoinAmount boobyPrize){
	std::vector<std::pair<Agentid,CoinAmount>> distVec;
	const int N = agentVec.size();
	if(N==0)
		return distVec;
	
	const int MinN1 = 1, MaxN1 = rule.topFirstCount;
	const int MinN2 = 0, MaxN2 = rule.topSecondCount;
	const int N1 = min(N,MaxN1);
	const int N2 = min(max(MinN2,N-MaxN1),MaxN2);
	assert(MinN1 <= N1 && N1 <= MaxN1);
	assert(MinN2 <= N2 && N2 <= MaxN2);

	if(N2 <= 0){
		topFirstTotalAmount += topSecondTotalAmount;
		topSecondTotalAmount = 0;
	}

	// TOP First: [1,topFirstCount]
	const CoinAmount topFirstAverage = topFirstTotalAmount/N1;
	const CoinAmount topFirstRemainder = topFirstTotalAmount%N1;
	for(int i=0; i<N1; i++){
		const Agentid id = agentVec[i].id;
		const CoinAmount reward = (i==0) ? (topFirstAverage + topFirstRemainder) : topFirstAverage;
		distVec.push_back(make_pair(id,reward));
	}

	// TOP Second: [topFirstCount+1,topFirstCount+topSecondCount]
	WeightNumber totalN2Weight = 0;
	for(int i=0; i<N2; i++){
		const Agent& agent = agentVec[N1+i];
		totalN2Weight += agent.weight;
	}
	CoinAmount topSecondRewardTotal = 0;
	CoinAmount topSecondTotalAmountRemain = topSecondTotalAmount;
	for(int i=0; i<N2; i++){
		int countIndex = N1+i;
		const Agent& agent = agentVec[countIndex];
		if(agent.weight == 0 && topSecondTotalAmountRemain >= boobyPrize){
			CoinAmount reward = boobyPrize;
			distVec.push_back(make_pair(agent.id,reward));
			topSecondRewardTotal += reward;
			topSecondTotalAmountRemain -= reward;
		}
	}
	for(int i=0; i<N2; i++){
		int countIndex = N1+i;
		const Agent& agent = agentVec[countIndex];
		CoinAmount reward = 0;
		if(totalN2Weight > 0 && agent.weight > 0){
			reward = WeightAmount(topSecondTotalAmountRemain,agent.weight,totalN2Weight);
			distVec.push_back(make_pair(agent.id,reward));
			topSecondRewardTotal += reward;
		}
	}
	const CoinAmount topSecondRemainder = topSecondTotalAmount - topSecondRewardTotal;
	if(topSecondRemainder > 0){
		distVec[N1+0].second += topSecondRemainder;
	}

	return distVec;
}

bool crp::LeftAgentMoreActive(const Agent& left, const Agent& right){
	if(left.weight != right.weight)
		return left.weight > right.weight;
	else if(left.blockAmount != right.blockAmount)
		return left.blockAmount > right.blockAmount;
	else 
		return left.lastBlockTime > right.lastBlockTime;
}

CoinReleasePlan& CoinReleasePlan::GetInstance(){
	static CoinReleasePlan sigleton;
	return sigleton;
}

bool crp::IsPowHeight(Height h){
	return crp::CoinReleasePlan::GetInstance().pow.heightRange.Exists(h);
}

bool crp::IsPosHeight(Height h){
	return crp::CoinReleasePlan::GetInstance().GetPosPlan(h).heightRange.Exists(h);
}

BlockValue crp::GetBlockValue(Height h){
	return crp::CoinReleasePlan::GetInstance().GetBlockValue(h);
}

Height crp::GetPosStartHeight(){
	return crp::CoinReleasePlan::GetInstance().pos_v1.heightRange.Begin();
}

// result = totalAmount * weight / totalWeight
int64_t crp::WeightAmount(int64_t totalAmount, uint256 weight, uint256 totalWeight) {
	const int64_t& a = totalAmount;
	const uint256& b = weight;
	const uint256& c = totalWeight;
	
	const bool premise = (a >= 0 && b >= 0 && c > 0 && b <= c);
	if(!premise){
		std::stringstream msg;
		msg << "assert failed: a >= 0 && b >= 0 && c > 0 && b <= c. ";
		msg << "info: ";
		msg << "a=" << a << ",b=" << b.ToString() << ",c=" << c.ToString();
		throw std::runtime_error(msg.str());
	}

	uint256 ua = a;
	uint256 result = ua * b / c;
	
	uint64_t uint64Value = result.Get64();
	if(uint64Value > std::numeric_limits<int64_t>::max()){
		std::stringstream msg;
		msg << "result greater than int64_t::max. ";
		msg << "info: ";
		msg << "uint64Value=" << uint64Value ;
		msg << ",max=" << std::numeric_limits<int64_t>::max();
		msg << ",totalAmount=" << totalAmount;
		msg << ",weight=" << weight.ToString();
		msg << ",totalWeight=" << totalWeight.ToString();
		throw std::runtime_error(msg.str());
	}
	
	return static_cast<int64_t>(uint64Value);
}


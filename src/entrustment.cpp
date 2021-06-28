#include "entrustment.h"
#include "primitives/block.h"
#include "main.h"
#include "util.h"
#include <sstream> //std::ostringstream
#include "utilmoneystr.h" //FormatMoney
#include "txdb.h"
#include "crp/CoinReleasePlan.h"
#include <boost/filesystem.hpp>
#include "entrustment_impl.h"
#include "scriptex.h"
#include "forkheights.h" // forkheight_release_v_2_5_1, forkheight_release_v_2_4_0

using namespace dpos;
using namespace entrustment_impl;

extern CBlockTreeDB* pblocktree;

static crp::DistributeRule GetDistributeRule(const BlockHeight& height, const BlockHeight& forkheight);

bool dpos::GetConfirmedTxOut(COutPoint output, CTxOut& txout)
{
	CTransaction tx;
	if(!FindConfirmedTransaction(output.hash, tx))
		return false;
	
	txout = tx.vout[output.n];
	return true;
}

bool dpos::FindConfirmedTransaction(const uint256& txid, CTransaction& txOut)
{
    CBlockIndex* pindexSlow = NULL;
    {
        LOCK(cs_main);

        if (fTxIndex) {
            CDiskTxPos postx;
            if (pblocktree && pblocktree->ReadTxIndex(txid, postx)) {
                CAutoFile file(OpenBlockFile(postx, true), SER_DISK, CLIENT_VERSION);
                if (file.IsNull())
                    return error("%s: OpenBlockFile failed", __func__);
                CBlockHeader header;
                try {
                    file >> header;
                    fseek(file.Get(), postx.nTxOffset, SEEK_CUR);
                    file >> txOut;
                } catch (std::exception& e) {
                    return error("%s : Deserialize or I/O error - %s", __func__, e.what());
                }
                if (txOut.GetHash() != txid)
                    return error("%s : txid mismatch", __func__);
                return true;
            }

            // transaction not found in the index, nothing more can be done
            return false;
        }

        int nHeight = -1;
        CCoinsViewCache& view = *pcoinsTip;
        const CCoins* coins = view.AccessCoins(txid);
        if (coins)
            nHeight = coins->nHeight;
        if (nHeight > 0)
            pindexSlow = chainActive[nHeight];
    }

    if (pindexSlow) {
        CBlock block;
        if (ReadBlockFromDisk(block, pindexSlow)) {
            BOOST_FOREACH (const CTransaction& tx, block.vtx) {
                if (tx.GetHash() == txid) {
                    txOut = tx;
                    return true;
                }
            }
        }
    }

    return false;
}

//////////////////////////////////////
UserStatistics::UserStatistics()
{
	version = Default;
	joinHeight = 0;
	entrustAmount = 0;
	totalDealDivideReward = 0;
	totalDividendReward = 0;
}
UniValue UserStatistics::ToUniValue() const
{
	UniValue root(UniValue::VOBJ);
	root.push_back(Pair("version",version));
	root.push_back(Pair("joinHeight",joinHeight));
	root.push_back(Pair("totalDealDivideReward",FormatMoney(totalDealDivideReward)));
	root.push_back(Pair("totalDividendReward",FormatMoney(totalDividendReward)));

	return root;
}
bool UserStatistics::OutputTo(ostream& os) const
{
	os << version << ' ' << joinHeight << ' ' << entrustAmount << ' ' << totalDealDivideReward << ' ' << totalDividendReward;
	return os.good();
}
bool UserStatistics::InputFrom(istream& is)
{
	is >> version >> joinHeight >> entrustAmount >> totalDealDivideReward >> totalDividendReward;
	return is.good();
}

ostream& operator<< (ostream& os, const dpos::UserStatistics& u)
{
	u.OutputTo(os);
	return os;
}

istream& operator>> (istream& is, dpos::UserStatistics& u)
{
	u.InputFrom(is);
	return is;
}

//////////////////////////////////////
Agent::Agent()
{
	version = Default;
	id = 0;
	receivedTotalAmount = 0;
	generatedBlockAmount = 0;
	lastBlockTime = 0;
	last_season_reward = 0;
}
Agent::Agent(AgentID id)
{
	version = Default;
	this->id = id;
	receivedTotalAmount = 0;
	generatedBlockAmount = 0;
	lastBlockTime = 0;
	last_season_reward = 0;
}

AgentID Agent::Id()const
{
	return id;
}
void Agent::SetName(const std::string& nm) {
	name = nm;
}

std::string Agent::Name() const {
	return name;
}

void Agent::AddBlock(int64_t blocktime) {
	generatedBlockAmount += 1;
	monthlyCount.blockAmount += 1;
	lastBlockTime = blocktime;
}

CAmount Agent::Amount()const
{
	return receivedTotalAmount;
}

void Agent::AddAmount(const CBitcoinAddress& user, CAmount amount)
{
	receivedTotalAmount += amount;
	UserStatistics& statistics = userStatistics[user];
	statistics.entrustAmount += amount;
	monthlyCount.entrustAmount += amount;
}
void Agent::DelAmount(const CBitcoinAddress& user, CAmount amount)
{
	receivedTotalAmount -= amount;
	UserStatistics& statistics = userStatistics[user];
	statistics.entrustAmount -= amount;
	monthlyCount.entrustAmount -= amount;
}
void Agent::AddDealDivideReward(BlockHeight height, CBitcoinAddress user, CAmount amount)
{
	UserStatistics& statistics = userStatistics[user];
	statistics.totalDealDivideReward += amount;
	//LogPrint("dpos","Agent::AddDealDivideReward | id=%s,height=%d,user=%s,+amount=%lld,totalDealDivideReward=%lld\n",id.ToString(),height,user.ToString(),amount,statistics.totalDealDivideReward);
}
void Agent::AddDividendReward(BlockHeight height, CBitcoinAddress user, CAmount amount)
{
	UserStatistics& statistics = userStatistics[user];
	statistics.totalDividendReward += amount;
	//LogPrint("dpos","Agent::AddDividendReward | id=%s,height=%d,user=%s,+amount=%lld,totalDividendReward=%lld\n",id.ToString(),height,user.ToString(),amount,statistics.totalDividendReward);
}
UniValue Agent::ToUniValue() const
{
	UniValue activeUsers(UniValue::VARR);
	for(std::map<CBitcoinAddress,UserStatistics>::const_iterator it = userStatistics.begin(); it != userStatistics.end(); it++)
	{
		const CBitcoinAddress& user = it->first;
		const UserStatistics& statistics = it->second;
		if(!Entrustment::GetInstance().IsActiveUser(statistics,chainActive.Height())) continue;
		UniValue obj(UniValue::VOBJ);
		obj.push_back(Pair(user.ToString(),FormatMoney(statistics.entrustAmount)));
		activeUsers.push_back(obj);
	}

	CAmount totalDividend(0),totalDealDividend(0);
	UniValue totalDivideRewardArray(UniValue::VARR);
	for(std::map<CBitcoinAddress,UserStatistics>::const_iterator it = userStatistics.begin(); it != userStatistics.end(); it++)
	{
		const CBitcoinAddress& user = it->first;
		const UserStatistics& statistics = it->second;
		UniValue obj(UniValue::VOBJ);
		UniValue statisticsObj = statistics.ToUniValue();
		statisticsObj.push_back(Pair("user",user.ToString()));
		obj.push_back(Pair(user.ToString(),statisticsObj));
		totalDivideRewardArray.push_back(obj);
		totalDividend += statistics.totalDividendReward;
		totalDealDividend += statistics.totalDealDivideReward;
	}
	UniValue monthlyCountObj(UniValue::VOBJ);
	monthlyCountObj.push_back(Pair("blockAmount",monthlyCount.blockAmount));
	monthlyCountObj.push_back(Pair("entrustAmount",monthlyCount.entrustAmount));

	UniValue root(UniValue::VOBJ);
	root.push_back(Pair("id",id.GetHex()));
	root.push_back(Pair("name",name));
	root.push_back(Pair("receivedTotalAmount",FormatMoney(receivedTotalAmount)));
	root.push_back(Pair("userAmounts.size",activeUsers.size()));
	root.push_back(Pair("userAmounts",activeUsers));
	root.push_back(Pair("totalDivideRewards.size",userStatistics.size()));
	root.push_back(Pair("totalDivideRewards",totalDivideRewardArray));
	root.push_back(Pair("totalDividend",totalDividend));
	root.push_back(Pair("totalDealDividend",totalDealDividend));
	root.push_back(Pair("generatedBlockAmount",generatedBlockAmount));
	root.push_back(Pair("lastBlockTime",lastBlockTime));
	root.push_back(Pair("monthlyCount",monthlyCountObj));

	return root;
}
CAmount Agent::GetTotalDealDivideAmount() const
{
	CAmount total = 0;
	for(std::map<CBitcoinAddress,UserStatistics>::const_iterator it = userStatistics.begin(); it != userStatistics.end(); it++)
	{
		const UserStatistics& statistics = it->second;
		total += statistics.totalDealDivideReward;
	}
	return total;
}
CAmount Agent::GetTotalDividendAmount() const
{
	CAmount total = 0;
	for(std::map<CBitcoinAddress,UserStatistics>::const_iterator it = userStatistics.begin(); it != userStatistics.end(); it++)
	{
		const UserStatistics& statistics = it->second;
		total += statistics.totalDividendReward;
	}
	return total;
}

int Agent::GetActiveUserCount() const
{
	int count = 0;
	for(std::map<CBitcoinAddress,UserStatistics>::const_iterator it = userStatistics.begin(); it != userStatistics.end(); it++)
	{
		if(Entrustment::GetInstance().IsActiveUser(it->second,chainActive.Height()))
			count++;
	}
	return count;
}

size_t Agent::MemorySize() const
{
	size_t size = sizeof(*this);
	size += (30); //name.length()==30
	size += ((sizeof(std::pair<CBitcoinAddress,UserStatistics>)+34)*userStatistics.size()); //address.length()==34
	return size;
}


//////////////////////////////////////////////////
Entrustment::Entrustment()
{
	version = Default;
	const crp::PosPlan& posplan = crp::CoinReleasePlan::GetInstance().GetPosPlan(0);
	createAgentAmount = 3000 * COIN;
	minEntrustAmount = 100 * COIN;
	minDivideReward = 2 * COIN;
	minDivideReward_v2 = 1 * COIN;
	assert(posplan.velocity > 0);
	entrustTxLockHeight = 72 * crp::HOUR / posplan.velocity;
	entrustWithCommentTxLockHeight = 2 * crp::MONTH / posplan.velocity;
	depriveTxLockHeight = 24 * crp::HOUR / posplan.velocity;
	baseHeightOffset = 60;
	forkHeightForLockMinerRewardReceiver = 34600;
	forkHeightForSpecifyMinerRewardReceiver = 74001;
	forkheight_halfMinEntrustAmount = forkheight_release_v_2_4_0;
	forkheight_halfCreateAgentAmount = forkheight_release_v_2_4_0;
	forkheight_halfMinDivideReward = forkheight_release_v_2_4_0;
	forkheight_seasonRewardTop150 = forkheight_release_v_2_5_1;
	forkheight_blackAgentList = forkheight_release_v_2_5_1;
}

CAmount Entrustment::GetMinDivideReward(BlockHeight h) const
{
	if(h >= forkheight_halfMinDivideReward)
		return GetMinDivideRewardV2();
	return GetMinDivideRewardV1();
}

CAmount Entrustment::GetMinDivideRewardV1() const { return minDivideReward; }
CAmount Entrustment::GetMinDivideRewardV2() const { return minDivideReward_v2; }

bool Entrustment::IsSeasonRewardV2(BlockHeight h) const
{
	return crp::CoinReleasePlan::GetInstance().IsSeasonRewardV2(h);
}

bool Entrustment::IsAgent() const
{
	return !myAgentId.IsNull();
}

Agent* Entrustment::GetAgent(BlockHeight h, AgentID id)
{
	if(agentMaps.find(h)==agentMaps.end())
		return nullptr;
	
	AgentMap& agentMap = agentMaps.at(h);
	if(agentMap.find(id)==agentMap.end())
		return nullptr;
	
	return &agentMap.at(id);
}
const Agent* Entrustment::GetAgent(BlockHeight h, AgentID id)const
{
	if(agentMaps.find(h)==agentMaps.end())
		return nullptr;
	
	const AgentMap& agentMap = agentMaps.at(h);
	if(agentMap.find(id)==agentMap.end())
		return nullptr;
	
	return &agentMap.at(id);
}
Agent* Entrustment::GetMyAgent(BlockHeight h)
{
	return GetAgent(h,GetMyAgentID());
}

std::map<CBitcoinAddress,CAmount> Entrustment::GetExpectDivideRewardMap(BlockHeight height, AgentID agentid) const
{
	std::map<CBitcoinAddress,CAmount> result;

	BlockHeight prevHeight = height - 1;
	BlockHeight baseheight = GetBaseHeight(height);
	
	if(baseheight < crp::GetPosStartHeight()){
		LogPrint("dpos","Entrustment::GetExpectDivideRewardMap | info: baseheight below pos startheight | height=%d, prevHeight=%d, baseheight=%d, agentid=%s\n",height,prevHeight,baseheight,agentid.ToString());
		return result;
	}
	
	const Agent* baseAgent = GetAgent(baseheight,agentid);
	if(!baseAgent){
		LogPrint("dpos","Entrustment::GetExpectDivideRewardMap | error, base agent not found | height=%d, prevHeight=%d, baseheight=%d, agentid=%s\n",height,prevHeight,baseheight,agentid.ToString());
		return result;
	}

	// [H].reward = [H-offset].dealDivide - [H-1].dividend
	std::map<CBitcoinAddress,UserStatistics>::const_iterator it = baseAgent->userStatistics.begin();
	for(; it != baseAgent->userStatistics.end(); it++) {
		const CBitcoinAddress& user = it->first;
		const UserStatistics& baseHeightDivide = it->second;
		const UserStatistics* prevHeightDivide = GetUserStatistics(prevHeight,agentid,user);
		if(prevHeightDivide == nullptr && height >= Params().forkheight_clearInactiveUser)
			continue;
		if(baseheight < prevHeightDivide->joinHeight && height >= Params().forkheight_addJoinHeight)
			continue;
		const CAmount prevDividend = prevHeightDivide ? prevHeightDivide->totalDividendReward : 0;
		if(!prevHeightDivide)
			LogPrint("dpos","Entrustment::GetExpectDivideRewardMap | error, prev divide not found | height=%d, prevHeight=%d, baseheight=%d, agentid=%s, user=%s\n",height,prevHeight,baseheight,agentid.ToString(),user.ToString());
		const CAmount reward = baseHeightDivide.totalDealDivideReward - prevDividend;
		if(reward >= GetMinDivideReward(height)){
			result[user] = reward;
		}
	}

	return result;
}

std::map<CBitcoinAddress,CAmount> Entrustment::GetDealDivideRewardMap(BlockHeight blockheight, AgentID agentid) const
{
	BlockHeight prevheight = blockheight - 1;
	return GetBonusMap(prevheight,agentid,crp::GetBlockValue(blockheight).entrust);
}

bool Entrustment::AddMetaBlock(BlockHeight height, DposMetaBlock& block)
{
	LOCK(cs_heights);

	if(IsHeightExists(height)){
		LogPrint("dpos","Entrustment::AddMetaBlock | height=%d, IsHeightExists=1\n",height);
		return false;
	}
	
	int64_t clock0 = GetTimeMicros();

	InitHeightAmount(height);
	
	int64_t clock1 = GetTimeMicros();

	if(47639 <= height && height <= 47642)
		LogPrint("dpos","Entrustment::AddMetaBlock | IsSeasonRewardHeight(%d)=%d\n",height,crp::CoinReleasePlan::GetInstance().IsSeasonRewardHeight(height));
	
	if(crp::CoinReleasePlan::GetInstance().IsSeasonRewardHeight(height)){
		StartNewMonth(height);
	}
	
	if(!block.agentid.IsNull())
	{
		Agent* stakeAgent = GetAgent(height,block.agentid); // find agent which create this block
		if(!stakeAgent){
			LogPrint("dpos","Entrustment::AddMetaBlock | agent not found | height=%d, agentid=%s\n",height,block.agentid.ToString());
			return false;
		}

		stakeAgent->AddBlock(block.time);
			
		//count DealDivideReward
		std::map<CBitcoinAddress,CAmount> userRewardMap = GetDealDivideRewardMap(height,block.agentid);
		//LogPrint("dpos","Entrustment::AddMetaBlock | info: count DealDivideReward | height=%d, coinstake->agentid=%s, userRewardMap.size=%u, stakeAgent->GetTotalDealDivideAmount=%lld\n",height,block.agentid.ToString(),userRewardMap.size(),stakeAgent->GetTotalDealDivideAmount());
		for(std::map<CBitcoinAddress,CAmount>::const_iterator it = userRewardMap.begin(); it != userRewardMap.end(); it++){
			stakeAgent->AddDealDivideReward(height,it->first,it->second);
		}

		//count DividendReward
		for(const DposMetaBlock::Reward& reward : block.reward_list){ 
			if(reward.type == int(DposMetaBlock::RewardType::ENTRUST))
				stakeAgent->AddDividendReward(height,CBitcoinAddress(reward.addr),reward.amount);
		}
	}

	int64_t clock2 = GetTimeMicros();

	for(const DposMetaBlock::Create& entrust : block.create_list){
		InitAgent(height,entrust.agentid,entrust.name);
	}
	for(const DposMetaBlock::Resign& resign : block.resign_list){
		DelAgent(height,resign.agentid);
	}
	for(const DposMetaBlock::Entrust& entrust : block.entrust_list)
	{
		if(entrust.type == int(DposMetaBlock::EntrustType::ENTRUST))
			AddEntrust(height,CBitcoinAddress(entrust.addr),entrust.agentid,entrust.amount);
		else 
			DepriveEntrust(height,CBitcoinAddress(entrust.addr),entrust.agentid,entrust.amount);
	}
	if(blackListStatus.count(height) > 0)
	{
		Blacklist& blist = blackListStatus[height];
		for(const AgentID& a : block.blackagent_list)
			blist.Add(a);
	}

	int64_t clock3 = GetTimeMicros();

	if(height >= Params().forkheight_clearInactiveUser) // fork to clear data
		ClearInactiveEntrustUser(height);

	CalcSeasonRewardFactor(height);

	CalcExpectRewardOfSeason(height);

	int64_t clock4 = GetTimeMicros();

	if(height > 0 && height%1000==0){
		WriteToBackupFile(height);
	}

	int64_t clock5 = GetTimeMicros();
	LogPrint("clock","Entrustment::AddMetaBlock | clock: init(0-1)=%lld, divide(1-2)=%lld, entrust(2-3)=%lld, season(3-4)=%lld, backup(4-5)=%lld, total(0-5)=%lld\n",clock1-clock0,clock2-clock1,clock3-clock2,clock4-clock3,clock5-clock4,clock5-clock0);

	return true;
}

void Entrustment::ClearInactiveEntrustUser(BlockHeight height)
{
	if(agentMaps.count(height) > 0)
	{
		AgentMap& agentMap = agentMaps[height];
		for(AgentMap::iterator agentMapIt = agentMap.begin(); agentMapIt != agentMap.end(); agentMapIt++)
		{
			std::vector<CBitcoinAddress> inactiveUsers;
			std::map<CBitcoinAddress,dpos::UserStatistics>& userStatistics = agentMapIt->second.userStatistics;
			for(const std::pair<CBitcoinAddress,UserStatistics>& userDivide : userStatistics)
			{
				if(!IsActiveUser(userDivide.second,height))
					inactiveUsers.push_back(userDivide.first);
			}
			for(const auto& user : inactiveUsers)
				userStatistics.erase(user);
		}
	}
}

bool Entrustment::IsActiveUser(const dpos::UserStatistics& user, BlockHeight height) const
{
	return user.entrustAmount > 0 || user.totalDealDivideReward - user.totalDividendReward >= GetMinDivideReward(height);
}

void Entrustment::CalcSeasonRewardFactor(BlockHeight height)
{
	assert(agentMaps.count(height) > 0);
	AgentMap& agentMap = agentMaps[height];
	for(AgentMap::iterator agentMapIt = agentMap.begin(); agentMapIt != agentMap.end(); agentMapIt++){
		Agent& stakeAgent = agentMapIt->second;
		stakeAgent.monthlyCount.reward_factor += stakeAgent.receivedTotalAmount;
	}
}

void Entrustment::CalcExpectRewardOfSeason(BlockHeight height)
{
	// Reset the expect_reward of all agents.
	AgentMapMap::iterator heightMapIt = agentMaps.find(height);
	if(heightMapIt != agentMaps.end())
	{
		for(AgentMap::iterator agentMapIt = heightMapIt->second.begin(); agentMapIt != heightMapIt->second.end(); agentMapIt++)
		{
			Agent& agent = agentMapIt->second;
			agent.monthlyCount.expect_reward = 0;
		}
	}

 	// Update the expect_reward of agent in reward_vec.
	const crp::Season season = crp::CoinReleasePlan::GetInstance().GetSeason(height);
	std::vector<std::pair<const Agent*,CAmount>> reward_vec = GetSeasonReward(height, season.reward_height);
	size_t size = reward_vec.size();
	
	//LogPrint("dpos","Entrustment::CalcExpectRewardOfSeason | height=%d, season.reward_height=%d, reward_vec.size()=%u\n",height,season.reward_height,reward_vec.size());
	for(int i=0; i<size; i++){
		Agent* agent = const_cast<Agent*>(reward_vec[i].first);
		CAmount reward = reward_vec[i].second;
		agent->monthlyCount.expect_reward = reward;
		//LogPrint("dpos","Entrustment::CalcExpectRewardOfSeason | i=%d, agent->name=%s, agent->monthlyCount.expect_reward=%lld\n",i,agent->name, agent->monthlyCount.expect_reward);
	}
}

void Entrustment::StartNewMonth(BlockHeight height)
{
	LogPrint("dpos","Entrustment::StartNewMonth | height=%d\n",height);
	AgentMapMap::iterator heightMapIt = agentMaps.find(height);
	if(heightMapIt == agentMaps.end())
		return;
	LogPrint("dpos","Entrustment::StartNewMonth | heightMapIt->second.size()=%u\n",heightMapIt->second.size());
	for(AgentMap::iterator agentMapIt = heightMapIt->second.begin(); agentMapIt != heightMapIt->second.end(); agentMapIt++){
		Agent& agent = agentMapIt->second;
		agent.last_season = agent.monthlyCount;
		LogPrint("dpos","Entrustment::StartNewMonth | agent.monthlyCount.expect_reward=%lld, agent.last_season.expect_reward=%lld\n",agent.monthlyCount.expect_reward, agent.last_season.expect_reward);
		agent.monthlyCount.StartNewMonth();
	}
	blackListStatus[height].Clear();
}

vector<Agent*> Entrustment::GetAgentsRank(BlockHeight height, const SortRule& sort_rule)
{
	vector<Agent*> rank;
	int top_count = GetDistributeRule(height, forkheight_seasonRewardTop150).TotalCount();

	AgentMapMap::iterator it = agentMaps.find(height);
	if(it == agentMaps.end())
		return rank;
	
	AgentMap& agentMap = it->second;
	for(AgentMap::iterator agentMapIt = agentMap.begin(); agentMapIt != agentMap.end(); agentMapIt++)
	{
		const Agent& agent = agentMapIt->second;
		if(AgentIsInBlacklist(agent.id,blackListStatus,height))
			continue;
		rank.push_back(&(agentMapIt->second));
	}

	SortComparer sort_comparer(sort_rule);
	sort(rank.begin(),rank.end(),sort_comparer);
	
	if(rank.size() > top_count)
		rank.resize(top_count);
	return rank;
}

void Entrustment::Lock()
{
	cs_heights.lock();
}

void Entrustment::Unlock()
{
	cs_heights.unlock();
}

void Entrustment::WriteToBackupFile(BlockHeight height)
{
	LogPrint("dpos","Entrustment::WriteToBackupFile | info | height=%d\n",height);
	std::string fname = GetBackupFileName(height);
	std::ofstream file(fname);
	if(!file.is_open()){
		LogPrint("dpos","Entrustment::WriteToBackupFile | warning, file '%s' isn't open | height=%d\n",fname,height);
		return;
	}

	file << version << endl;

	AgentMapMap::const_iterator heightMapIt = agentMaps.find(height);
	if(heightMapIt != agentMaps.end()){
		const AgentMap& agentMap = heightMapIt->second;
		file << height << endl;
		LogPrint("dpos","Entrustment::WriteToBackupFile | Write %u agent at height %d ...\n",agentMap.size(),height);
		WriteAgentMapToFile(agentMap,file);
		Blacklist blist;
		if(blackListStatus.count(height) > 0)
			blist = blackListStatus[height];
		WriteBlacklistToFile(blist,file);
	}
}

bool Entrustment::WriteAgentMapToFile(const AgentMap& agentMap, std::ofstream& file)
{
	const int agentCount = agentMap.size();
	file << agentCount << endl;
	for(AgentMap::const_iterator it = agentMap.begin(); it != agentMap.end(); it++){
		const Agent& agent = it->second;
		file << agent << endl;
	}
}

bool read_entrustment_v0(std::istream& is, BlockHeight height, int agentCount, AgentMapMap& agentMaps)
{
	LogPrint("dpos","read_entrustment_v0 | Read %d agent at height %d ...\n",agentCount,height);
	return read_agent_map_from_istream(is,agentCount,agentMaps[height]);
}

bool read_entrustment_v1(std::istream& is, AgentMapMap& agentMaps)
{
	int heightCount = 0, height = 0, agentCount = 0;
	is >> heightCount;
	while(heightCount--){
		is >> height;
		is >> agentCount;
		LogPrint("dpos","read_entrustment_v1 | Read %d agent at height %d ...\n",agentCount,height);
		bool ok = read_agent_map_from_istream(is,agentCount,agentMaps[height]);
		if(!ok)
			return false;
	}
	return true;
}

bool Entrustment::ReadFromBackupFile(BlockHeight height)
{
	LogPrint("dpos","Entrustment::ReadFromBackupFile | info | height=%d\n",height);
	std::string fname = GetBackupFileName(height);
	std::ifstream file(fname);
	if(!file.is_open()){
		LogPrint("dpos","Entrustment::ReadFromBackupFile | warning, file '%s' isn't open | height=%d\n",fname,height);
		return false;
	}

	int dataVersion = 0;
	file >> dataVersion;

	if(dataVersion >= Entrustment::V3)
		return read_entrustment_v3(file,agentMaps,blackListStatus);
	if(dataVersion >= Entrustment::V2)
		return read_entrustment_v2(file,agentMaps,blackListStatus);
	if(dataVersion >= Entrustment::V1)
		return read_entrustment_v1(file,agentMaps);
	return read_entrustment_v0(file,height,version,agentMaps);
}

bool Agent::SerializeToText(std::ostream& os) const
{
	const Agent& a = *this;

	os << a.version << endl;
	
	os << a.id.ToString() << endl;
	
	os << (a.name.empty() ? std::string("null") : a.name) << endl;
	
	os << a.receivedTotalAmount << endl;

	os << a.userStatistics.size() << endl;
	for(const std::pair<CBitcoinAddress,UserStatistics>& userDivide : a.userStatistics){
		const UserStatistics& divide = userDivide.second;
		os << userDivide.first.ToString() << ' ' << divide << endl;
	}
	
	os << a.generatedBlockAmount << endl;
	
	os << a.lastBlockTime << endl;

	os << a.last_season.blockAmount << ' ' << a.last_season.entrustAmount << ' ' << a.last_season.reward_factor.ToString() << ' ' << a.last_season.expect_reward << endl;
	os << a.monthlyCount.blockAmount << ' ' << a.monthlyCount.entrustAmount << ' ' << a.monthlyCount.reward_factor.ToString() << ' ' << a.monthlyCount.expect_reward << endl;

	os << a.last_season_reward;
	return true;
}

std::ostream& operator<<(std::ostream& os, const Agent& a)
{
	a.SerializeToText(os);
	return os;
}

bool UnserializeAgentFromText_Part1(std::istream& is, Agent& a);
bool UnserializeAgentFromText_Part2(std::istream& is, Agent& a);

bool UnserializeAgentFromText_Part1(std::istream& is, Agent& a)
{
	std::string str;
	int size = 0;
	int64_t num = 0, num2 = 0;
	
	is >> str;
	//LogPrint("dpos","std::istream& operator>>(std::istream& is, Agent& a) | info | str=%s\n",str);
	a.id.SetHex(str);

	//is >> a.name; this can not handle SPACE in name
	while(1){
		char ch = is.peek();
		if(ch == '\r' || ch == '\n'){
			//LogPrint("dpos","std::istream& operator>>(std::istream& is, Agent& a) | info | ignore name char: %d\n",int(ch));
			is.ignore();
			continue;
		}
		break;
	}
	std::getline(is,a.name);
	if(a.name.size() > 0 && a.name.back() == '\r')
		a.name.resize(a.name.size()-1);
	if(a.name == "null")
		a.name.clear();
	//LogPrint("dpos","std::istream& operator>>(std::istream& is, Agent& a) | info | name=%s\n",a.name);
	
	LogPrint("dpos","std::istream& operator>>(std::istream& is, Agent& a) | info | agentid=%s, name=%s\n",a.id.ToString(),a.name);	

	is >> a.receivedTotalAmount;
	//LogPrint("dpos","std::istream& operator>>(std::istream& is, Agent& a) | info | receivedTotalAmount=%lld\n",a.receivedTotalAmount);
	
	return true;
}

bool Agent::UnserializeFromTextWithoutVersion(std::istream& is)
{
	Agent& a = *this;
	
	std::string str;
	int size = 0;
	int64_t num = 0, num2 = 0;

	UnserializeAgentFromText_Part1(is,*this);

	is >> size;
	//LogPrint("dpos","std::istream& operator>>(std::istream& is, Agent& a) | info | outpoints.size=%d\n",size);
	while(size-- > 0){
		is >> str >> num;
	}

	is >> size;
	//LogPrint("dpos","std::istream& operator>>(std::istream& is, Agent& a) | info | entrust.size=%d\n",size);
	while(size-- > 0){
		is >> str >> num;
		UserStatistics staticstics;
		staticstics.entrustAmount = num;
		a.userStatistics.insert(std::make_pair(CBitcoinAddress(str),staticstics));
	}
	
	is >> size;
	while(size-- > 0){
		is >> str >> num >> num2;
		UserStatistics& staticstics = a.userStatistics[CBitcoinAddress(str)];
		staticstics.totalDealDivideReward = num;
		staticstics.totalDividendReward = num2;
	}

	UnserializeAgentFromText_Part2(is,*this);

	return true;
}


bool UnserializeAgentFromText_Part2(std::istream& is, Agent& a)
{
	std::string str;
	int size = 0;
	int64_t num = 0, num2 = 0;

	is >> a.generatedBlockAmount;
	//LogPrint("dpos","std::istream& operator>>(std::istream& is, Agent& a) | info | generatedBlockAmount=%d\n",a.generatedBlockAmount);
	
	is >> a.lastBlockTime;
	//LogPrint("dpos","std::istream& operator>>(std::istream& is, Agent& a) | info | lastBlockTime=%lld\n",a.lastBlockTime);

	is >> a.last_season.blockAmount >> a.last_season.entrustAmount >> str >> a.last_season.expect_reward;
	a.last_season.reward_factor.SetHex(str);
	
	is >> a.monthlyCount.blockAmount >> a.monthlyCount.entrustAmount >> str >> a.monthlyCount.expect_reward;
	a.monthlyCount.reward_factor.SetHex(str);
	//LogPrint("dpos","std::istream& operator>>(std::istream& is, Agent& a) | info | a.monthlyCount: blockAmount=%d, entrustAmount=%lld\n",a.monthlyCount.blockAmount,a.monthlyCount.entrustAmount);

	is >> a.last_season_reward;

	return true;
}

bool Agent::UnserializeFromText(std::istream& is)
{
	Agent& a = *this;

	is >> a.version;
	
	std::string str;
	int size = 0;
	int64_t num = 0, num2 = 0;
	
	UnserializeAgentFromText_Part1(is,*this);
	
	is >> size;
	while(size-- > 0){
		is >> str;
		UserStatistics& staticstics = a.userStatistics[CBitcoinAddress(str)];
		is >> staticstics;
	}

	UnserializeAgentFromText_Part2(is,*this);
	return is.good();
}

std::istream& operator>>(std::istream& is, Agent& a)
{
	a.UnserializeFromText(is);
	return is;
}

std::string Entrustment::GetBackupFileName(BlockHeight height)
{
	boost::filesystem::path dir = GetDataDir()/"entrustment";
	boost::filesystem::create_directories(dir);
	
 	std::string fname = strprintf("entrustment-%d.txt",height);
	boost::filesystem::path path = dir/fname;
	return path.string();
}

UserStatistics* Entrustment::GetUserStatistics(BlockHeight height, const AgentID& agentid, const CBitcoinAddress& user)
{
	AgentMapMap::iterator agentMapIt = agentMaps.find(height);
	if(agentMapIt == agentMaps.end()) return nullptr;
	
	AgentMap::iterator agentIt = agentMapIt->second.find(agentid);
	if(agentIt == agentMapIt->second.end()) return nullptr;
	
	std::map<CBitcoinAddress,UserStatistics>::iterator divideIt = agentIt->second.userStatistics.find(user);
	if(divideIt == agentIt->second.userStatistics.end()) return nullptr;
	
	return &divideIt->second;
}

const UserStatistics* Entrustment::GetUserStatistics(BlockHeight height, const AgentID& agentid, const CBitcoinAddress& user) const
{
	AgentMapMap::const_iterator agentMapIt = agentMaps.find(height);
	if(agentMapIt == agentMaps.end()) return nullptr;
	
	AgentMap::const_iterator agentIt = agentMapIt->second.find(agentid);
	if(agentIt == agentMapIt->second.end()) return nullptr;
	
	std::map<CBitcoinAddress,UserStatistics>::const_iterator divideIt = agentIt->second.userStatistics.find(user);
	if(divideIt == agentIt->second.userStatistics.end()) return nullptr;
	
	return &divideIt->second;
}

void Entrustment::InitHeightAmount(BlockHeight height)
{
	assert(height >= 0);
	//LogPrintf("DPOS Entrustment::InitHeightAmount | height=%d\n",height);

	if(agentMaps.find(height) != agentMaps.end())
		return; //Already initialized

	//Copy all data from the previous height to the new height
	if(height == 0 || agentMaps.find(height-1)==agentMaps.end())
		agentMaps[height];
	else
		agentMaps[height] = agentMaps[height-1];

	const size_t saveEntryCount = GetArg("-checkblocks", 100) + 3;
	const BlockHeight lowerBound = height - saveEntryCount + 1;
	for(BlockHeight h = LowestHeight(); h < lowerBound; h++){
		agentMaps.erase(h);
	}

	size_t total_size = 0;
	for(std::map<BlockHeight,std::map<AgentID,Agent>>::const_iterator it1 = agentMaps.begin(); it1 != agentMaps.end(); it1++){
		for(std::map<AgentID,Agent>::const_iterator it = it1->second.begin(); it != it1->second.end(); it++){
			const Agent& a = it->second;
			total_size += a.MemorySize();
		}
	}
	LogPrint("dpos","Entrustment::InitHeightAmount|height=%d,agentMaps.size()=%lld,total_size=%lld\n",height,(int64_t)agentMaps.size(),(int64_t)total_size);

	CopyBlacklistStatus(blackListStatus,height-1,height,saveEntryCount);
}

BlockHeight Entrustment::LowestHeight() const
{
	BlockHeight minHeight = std::numeric_limits<BlockHeight>::max();
	for(std::map<BlockHeight,AgentMap>::const_iterator it = agentMaps.begin(); it != agentMaps.end(); it++)
	{
		minHeight = std::min(it->first,minHeight);
	}

	return minHeight;
}

bool Entrustment::IsHeightExists(BlockHeight height) const
{
	return agentMaps.find(height) != agentMaps.end();
}

BlockHeight  Entrustment::GetBaseHeight(BlockHeight newHeight) const
{
	if(newHeight < baseHeightOffset)
		LogPrint("dpos","Entrustment::GetBaseHeight | warning: newHeight=%d < baseHeightOffset=%d\n",newHeight, baseHeightOffset);

	return newHeight - baseHeightOffset;
}

AgentMap* Entrustment::GetAgentMapWithMaxHeight()
{
	const BlockHeight invalid = -1;
	BlockHeight h = invalid;
	for(std::map<BlockHeight,AgentMap>::iterator it = agentMaps.begin(); it != agentMaps.end(); it++)
	{
		h = std::max(it->first,h);
	}
	if(h == invalid)
		return nullptr;

	return &agentMaps[h];
}

const AgentMap* Entrustment::GetAgentMapWithMaxHeight() const
{
	const BlockHeight invalid = -1;
	BlockHeight h = invalid;
	for(std::map<BlockHeight,AgentMap>::const_iterator it = agentMaps.begin(); it != agentMaps.end(); it++)
	{
		h = std::max(it->first,h);
	}
	if(h == invalid)
		return nullptr;

	return &agentMaps.at(h);
}

bool Entrustment::AddEntrust(BlockHeight height, CBitcoinAddress addr, AgentID agentid, CAmount amount)
{
	Agent* agent = GetAgent(height,agentid);
	if(!agent)
		return false;

	if(agent->userStatistics[addr].entrustAmount == 0 && height >= Params().forkheight_addJoinHeight)
		agent->userStatistics[addr].joinHeight = height;
		
	agent->AddAmount(addr,amount);
	return true;
}

bool Entrustment::DepriveEntrust(BlockHeight height, CBitcoinAddress addr, AgentID agentid, CAmount amount)
{
	Agent* agent = GetAgent(height,agentid);
	if(!agent)
		return false;

	agent->DelAmount(addr,amount);
	return true;
}

std::vector<Agent> Entrustment::GetAgentList(BlockHeight h)const
{
	std::vector<Agent> ret;

	CCriticalSection& cs = const_cast<Entrustment*>(this)->cs_heights;
	LOCK(cs);
	
	if(agentMaps.count(h) == 0)
		return ret;

	const std::map<AgentID,Agent>& agents = agentMaps.at(h);
	for(const std::pair<AgentID,Agent>& pair : agents)
	{
		ret.push_back(pair.second);
	}
	return ret;
}

void Entrustment::SetMyAgentID(AgentID id)
{
	assert(myAgentId==AgentID(0));
	myAgentId = id;
}
AgentID Entrustment::GetMyAgentID()const
{
	return myAgentId;
}
CAmount Entrustment::GetCreateAgentAmount(BlockHeight blockheight) const
{
	if(blockheight >= forkheight_halfCreateAgentAmount)
		return createAgentAmount / 2;
	return createAgentAmount;
}
CAmount Entrustment::GetMinEntrustAmount(BlockHeight blockheight) const
{
	if(blockheight >= forkheight_halfMinEntrustAmount)
		return 30 * COIN;
	return minEntrustAmount;
}

BlockHeight Entrustment::GetEntrustTxLockHeight() const
{
	return entrustTxLockHeight;
}
BlockHeight Entrustment::GetEntrustWithCommentTxLockHeight(BlockHeight chainHeight) const
{
	if (chainHeight < forkheight_release_v_2_7_0) {
		return entrustTxLockHeight;
	}
	return entrustWithCommentTxLockHeight;
}
BlockHeight Entrustment::GetDepriveTxLockHeight() const
{
	return depriveTxLockHeight;
}

std::map<CBitcoinAddress,CAmount> Entrustment::GetBonusMap(BlockHeight height, AgentID agentid, CAmount blockBonus) const
{
	std::map<CBitcoinAddress,CAmount> bonusMap;

	const Agent* pAgent = GetAgent(height,agentid);
	if(pAgent==nullptr)
		return bonusMap;
	
	const Agent& agent = *pAgent;
	if(agent.receivedTotalAmount < 0)
		throw std::runtime_error("agent.receivedTotalAmount < 0");
	
	if(agent.receivedTotalAmount == 0)
		return bonusMap;
	
	CAmount totalBonus = 0;
	for(std::map<CBitcoinAddress,UserStatistics>::const_iterator it = agent.userStatistics.begin(); it != agent.userStatistics.end(); it++)
	{
		const CBitcoinAddress& user = it->first;
		const CAmount& entrust_amount = it->second.entrustAmount;
		if(entrust_amount <= 0){
			if(entrust_amount < 0)
				LogPrint("dpos","Entrustment::GetBonusMap | error | entrust_amount=%lld < 0, user=%s\n",entrust_amount,user.ToString());
			continue;
		}
		const CAmount bonus_amount = crp::WeightAmount(blockBonus,entrust_amount,agent.receivedTotalAmount);
		bonusMap[user] = bonus_amount;
		totalBonus += bonus_amount;
	}

	if(totalBonus < blockBonus && !bonusMap.empty())
	{
		bonusMap.begin()->second += (blockBonus - totalBonus);
	}
	
	return bonusMap;
}

bool Entrustment::CheckCoinStake(BlockHeight blockheight, const CTransaction& coinstake)
{
    int64_t clock_begin = GetTimeMicros();
	if(!coinstake.IsCoinStake()){
		LogPrint("dpos","Entrustment::CheckCoinStake | warning | tx is not coinstake\n");
		return false;
	}
	
	if(coinstake.vout.empty()){
		LogPrint("dpos","Entrustment::CheckCoinStake | warning | vout is empty\n");
		return false;
	}
	
	if(!GetBoolArg("-entrustrewardcheck", true))
		return true;
	
	BlockHeight baseheight = GetBaseHeight(blockheight);
	if(baseheight < LowestHeight())
		return true;
	
	AgentID agentid = coinstake.agentid;

	CScript creatorScript, receiverScript;
	creatorScript = GetCreatorPubKeyOf(agentid);
	vector<CBitcoinAddress> creatorAddrs = GetAddressesInScript(creatorScript);
	if( blockheight >= forkHeightForLockMinerRewardReceiver && coinstake.vout.size() > 2 ){
		receiverScript = coinstake.vout[2].scriptPubKey; // vout[1] is for contract
		vector<CBitcoinAddress> receiverAddrs = GetAddressesInScript(receiverScript);
		if( creatorAddrs.size() > 0 &&  receiverAddrs.size() > 0 &&	!(creatorAddrs[0] == receiverAddrs[0]) ) //CBitcoinAddress isn't implement "!="
		{
			LogPrint("dpos","Entrustment::CheckCoinStake | warning 1 | receiver of dpos-miner reward isn't match with creator. | agentid=%s, creatorScript=%s, receiverScript=%s, creatorAddrs[0]=%s, receiverAddrs[0]=%s\n",agentid.ToString(), creatorScript.ToString(), receiverScript.ToString(), creatorAddrs[0].ToString(), receiverAddrs[0].ToString());
			return false;
		}
	}
	
	const BlockHeight chainHeight = chainActive.Height();
	if(!GetAgent(chainHeight,agentid)){
		LogPrint("dpos","Entrustment::CheckCoinStake | warning | agent not found | chainHeight=%d,blockheight=%d,agentid=%s\n",chainHeight,blockheight,agentid.ToString());
		return false;
	}
	
    int64_t clock_divide = GetTimeMicros();
	std::map<CBitcoinAddress,CAmount> expectRewardMap = GetExpectDivideRewardMap(blockheight,agentid);
    int64_t clock_month = GetTimeMicros(); 
	std::map<CBitcoinAddress,CAmount> expectMonthlyRewardMap = GetMonthlyRewardMap(blockheight);

    int64_t clock_getagent = GetTimeMicros();
	const Agent* agent = GetAgent(baseheight,agentid);
	if(agent)
		LogPrint("dpos","Entrustment::CheckCoinStake | info | blockheight=%d, baseheight=%d, agentid=%s, entrustDivideMap.size=%u, receivedTotalAmount=%lld, TotalDealDivide=%lld, TotalDividend=%lld\n",blockheight,baseheight,agentid.ToString(),expectRewardMap.size(),agent->receivedTotalAmount,agent->GetTotalDealDivideAmount(),agent->GetTotalDividendAmount());
	else
		LogPrint("dpos","Entrustment::CheckCoinStake | warning | agent not found | blockheight=%d, baseheight=%d, entrustDivideMap.size=%u\n",blockheight,baseheight,expectRewardMap.size());
	
    int64_t clock_iteratevout = GetTimeMicros();
	for(std::vector<CTxOut>::const_iterator it = coinstake.vout.begin(); it != coinstake.vout.end(); it++){
		const CTxOut& txout = *it;
		if(txout.type != CTxOut::Type::ENTRUST_REWARD && txout.type != CTxOut::Type::MONTHLY_REWARD)
			continue;

		// check entrust and monthly reward
		
		std::vector<CBitcoinAddress> addresses = txout.GetScriptAddresses();
		if(addresses.empty()){
			LogPrint("dpos","Entrustment::CheckCoinStake | warning | blockheight=%d, addresses.empty(), txout=%s\n",blockheight,txout.ToString());
			return false;
		}

		CBitcoinAddress user = addresses[0];
		CAmount expectedValue = 0;
		if(txout.type == CTxOut::Type::ENTRUST_REWARD){
			if(expectRewardMap.count(user) == 0){
				LogPrint("dpos","Entrustment::CheckCoinStake | warning | blockheight=%d, user(%s) not found in expectRewardMap | expectRewardMap.size()=%lld\n",blockheight,user.ToString(),expectRewardMap.size());
				return false;
			}
			expectedValue = expectRewardMap.at(user);
		}
		else if(txout.type == CTxOut::Type::MONTHLY_REWARD){
			if(expectMonthlyRewardMap.count(user) == 0){
				LogPrint("dpos","Entrustment::CheckCoinStake | warning | blockheight=%d, user(%s) not found in expectMonthlyRewardMap | expectMonthlyRewardMap.size()=%lld\n",blockheight,user.ToString(),expectMonthlyRewardMap.size());
				return false;
			}
			expectedValue = expectMonthlyRewardMap.at(user);
		}
		else {
			// do not check other type of output
			continue;
		}
		const CAmount actualValue = txout.nValue;
		if(actualValue > expectedValue){
			LogPrint("dpos","Entrustment::CheckCoinStake | warning | actualValue > expectedValue, blockheight=%d, user=%s, actualValue=%lld, expectedValue=%lld\n",blockheight,user.ToString(), actualValue, expectedValue);
			return false;
		}
	}
    int64_t clock_end = GetTimeMicros();
	LogPrint("clock","Entrustment::CheckCoinStake | info, clock in micro-second | GetExpectDivideRewardMap=%lld, GetMonthlyRewardMap=%lld, iteratevout=%lld, total=%lld\n", clock_month-clock_divide, clock_getagent-clock_month, clock_end-clock_iteratevout, clock_end-clock_begin);
	return true;
}

bool dpos::HaveDposArgv(const std::map<std::string,std::string>& argsMap)
{
	return argsMap.count("-agentid") > 0;
}

bool Entrustment::CheckTransaction(const CTransaction& tx) const
{
	bool isEntrust = tx.IsEntrust();
	bool isDeprive = tx.IsDeprive();

	//Fixme
	std::vector <std::string> txWhiteList({
		"416cd2a3d1a2344ae389ee254ac58cc2535e6aa2fc246ff2e160d3e2f454f5e5",
		"ba2d9be86d8527d2c1729940f1cd908028a005c9f92e1e0eb05b15c032e00686",
		"1dc50daeda604027e781c1000c995adaae123fe22aeb6ff88812ce0ea4d6c732",
		"c58dfdbeb60e121ea47c3486fe56034ed956f778dc8659a3244c52d744e510fd",
		"aef9caa509d41ddb7d166ebdf4cac5bcf993423d66910cbed91f073a69514d7e"}
	);
	for (auto txid = txWhiteList.begin(); txid != txWhiteList.end(); txid++){
		if (tx.GetHash().ToString() == *txid){
			LogPrint("dpos","Bugly transaction found, issued a pass.");
			return true;
		}		
	}

	if(isEntrust || isDeprive)
	{
		if(!IsAgentIdValid(tx.agentid))
		{
			LogPrintf("Entrustment::CheckTransaction | invalid agentid | tx=%s\n",tx.ToString());
			return false;
		}
	}

	for(const CTxIn& txin : tx.vin)
	{
		CTransaction prevtx;
		if(!FindConfirmedTransaction(txin.prevout.hash, prevtx))
			continue; // fixme

		CTxOut prevout;
		if(txin.prevout.n < prevtx.vout.size())
			prevout = prevtx.vout[txin.prevout.n];

		BlockHeight currentBlockHeight = chainActive.Height();
		if(!isDeprive && prevout.IsEntrusted() && currentBlockHeight > 128150){ // there are error tx before 128151 in mainnet
			LogPrintf("Entrustment::CheckTransaction | prevout was entrusted | tx=%s, txin.prevout=%s, prevout=%s\n",tx.GetHash().ToString(),txin.prevout.ToString(),prevout.ToString());
			return false;
		}
		
		if(isDeprive && !prevout.IsEntrusted())
		{
			LogPrintf("Entrustment::CheckTransaction | prevout isn't entrusted | tx=%s, txin.prevout=%s, prevout=%s\n",tx.GetHash().ToString(),txin.prevout.ToString(),prevout.ToString());
			return false;
		}

		if(isDeprive && prevout.IsEntrusted() && currentBlockHeight > 138467) // begin check start with height-138467 in mainnet
		{
			if(IsEntrustTxIsLocking(txin.prevout.hash,chainActive.Height()))
			{
				LogPrintf("Entrustment::CheckTransaction | entrust tx is locking | tx=%s, txin.prevout=%s, prevout=%s, chainHeight=%d\n",tx.GetHash().ToString(),txin.prevout.ToString(),prevout.ToString(),chainActive.Height());
				return false;
			}
		}

		if(Params().forkheight_lockDepriveTx <= currentBlockHeight && currentBlockHeight < Params().forkheight_cancelLockDepriveTx)
		{
			if(prevtx.IsDeprive() && IsDepriveTxIsLocking(txin.prevout.hash,chainActive.Height()))
			{
				LogPrintf("Entrustment::CheckTransaction | deprive tx is locking | tx=%s, txin.prevout=%s, prevout=%s, chainHeight=%d\n",tx.GetHash().ToString(),txin.prevout.ToString(),prevout.ToString(),chainActive.Height());
				return false;
			}
		}
	}
	
	bool foundCreateOutput = false;
	for(int i=0; i<tx.vout.size(); i++)
	{
		CTxOut txout = tx.vout[i];
		if(txout.IsCreateAgentType())
		{
			CAmount amountLimit = GetCreateAgentAmount(chainActive.Height()+1);
			if(txout.nValue < amountLimit) {
				LogPrint("dpos","Entrustment::CheckTransaction | CreateAgent output: txout.nValue(%lld) < amountLimit(%lld)\n",txout.nValue,amountLimit);
				return false;
			}
			foundCreateOutput = true;
		}
		else if(txout.IsEntrusted())
		{
			BlockHeight predictHeight = chainActive.Height() + 1;
			if(predictHeight >= forkheight_halfMinEntrustAmount)
			{
				CAmount amountLimit = GetMinEntrustAmount(predictHeight);
				if(txout.nValue < amountLimit) {
					LogPrint("dpos","Entrustment::CheckTransaction | Entrust output: txout.nValue(%lld) < amountLimit(%lld)\n",txout.nValue,amountLimit);
					return false;
				}
			}
		}
	}

	if(!foundCreateOutput && tx.IsCreateAgent())
	{
		LogPrint("dpos","Entrustment::CheckTransaction | invalid create agent tx\n");
		return false;
	}

	return true;
}

bool Entrustment::IsEntrustTxIsLocking(const uint256& txid, BlockHeight chainHeight) const
{
	CTransaction tx;
	uint256 blockhash;
	if(!GetTransaction(txid, tx, blockhash, true))
	{
		LogPrintf("Entrustment::IsEntrustTxIsLocking | tx not found | txid=%s, chainHeight=%d\n",txid.ToString(),chainHeight);
		return true;
	}
	return IsEntrustTxLocking(tx, chainHeight, blockhash);
}

static BlockHeight GetTxHeight(const uint256& txid)
{
	CTransaction tx;
	uint256 blockhash;
	const BlockHeight invalidHeight = -1;
	if(!GetTransaction(txid, tx, blockhash, true))
		return invalidHeight;
	if(!mapBlockIndex.count(blockhash))
		return invalidHeight;
	return mapBlockIndex[blockhash]->nHeight;
}

bool Entrustment::IsDepriveTxIsLocking(const uint256& txid, BlockHeight chainHeight) const
{
	const BlockHeight txheight = GetTxHeight(txid);
	if(txheight < 0)
	{
		//LogPrintf("Entrustment::IsDepriveTxIsLocking | height of tx not found | txid=%s\n",txid.ToString());
		return true;
	}
	const BlockHeight confirm = chainHeight - txheight;
	if(confirm < GetDepriveTxLockHeight())
	{
		//LogPrintf("Entrustment::IsDepriveTxIsLocking | confirm not enough | txid=%s, chainHeight=%d, txheight=%d, confirm=%d, lockheight=%d\n",txid.ToString(),chainHeight,txheight,confirm,GetDepriveTxLockHeight());
		return true;
	}
	return false;
}

//Clear all data above height (not included)
void Entrustment::RevertToHeight(BlockHeight height)
{
	LOCK(cs_heights);
	
	LogPrint("dpos","Entrustment::RevertToHeight | height=%d\n",height);
	BlockHeight h = height + 1;
	while(agentMaps.count(h) > 0)
	{
		agentMaps.erase(h);
		h++;
	}
}

std::ostream& operator<< (std::ostream& os, const Entrustment& e)
{
	return os << e.ToString();
}

std::string Entrustment::ToString() const
{
	UniValue value = ToUniValue();
	return value.write();
}

UniValue Entrustment::ToUniValue() const
{
	UniValue heightMapArray(UniValue::VARR);
	for(AgentMapMap::const_iterator heightIt = agentMaps.begin(); heightIt != agentMaps.end(); heightIt++)
	{
		BlockHeight height = heightIt->first;
		const AgentMap& agentMap = heightIt->second;
		
		UniValue agentMapArray(UniValue::VARR);
		for(AgentMap::const_iterator agentIt = agentMap.begin(); agentIt != agentMap.end(); agentIt++)
		{
			AgentID agentid = agentIt->first;
			const Agent& agent = agentIt->second;
			
			UniValue agentObj(UniValue::VOBJ);
			agentObj.push_back(Pair(agentid.GetHex(),agent.ToUniValue()));
			agentMapArray.push_back(agentObj);
		}

		UniValue heightObj(UniValue::VOBJ);
		heightObj.push_back(Pair(std::to_string(height),agentMapArray));
		
		heightMapArray.push_back(heightObj);
	}

	BlockHeight chainheight = chainActive.Height();
	UniValue root(UniValue::VOBJ);
	root.push_back(Pair("myAgentId",myAgentId.GetHex()));
	root.push_back(Pair("createAgentAmount",FormatMoney(GetCreateAgentAmount(chainheight))));
	root.push_back(Pair("minEntrustAmount",FormatMoney(GetMinEntrustAmount(chainheight))));
	root.push_back(Pair("minDivideReward",FormatMoney(GetMinDivideReward(chainheight))));
	root.push_back(Pair("depriveLockHeight",entrustTxLockHeight));
	root.push_back(Pair("agentMaps.size",agentMaps.size()));
	root.push_back(Pair("agentMaps",heightMapArray));

	return root;
}

bool Entrustment::IsAgentIdValid(AgentID id) const
{
 	const AgentMap* agentMap = GetAgentMapWithMaxHeight();
	if(agentMap == nullptr)
		return false;
	return agentMap->count(id) > 0;
}

bool Entrustment::InitAgent(BlockHeight h, AgentID id, const std::string& name)
{
	if(GetAgent(h,id) == nullptr)
	{
		Agent a(id);
		a.SetName(name);
		agentMaps[h][id] = a;
		return true;
	}
	return false;
}
void Entrustment::DelAgent(BlockHeight h, AgentID id)
{
	//LogPrintf("DPOS Entrustment::DelAgent | h=%d, id=%s\n",h,id.ToString());
	if(agentMaps.find(h)==agentMaps.end())
		return;
	
	std::map<AgentID,Agent>& agents = agentMaps.at(h);
	if(agents.find(id)==agents.end())
		return;

	agents.erase(id);
}

bool dpos::CheckCoinStake(const CBlock& block)
{
	const CTransaction* coinstake = block.GetCoinStake(); 
	if(!coinstake){
		LogPrint("dpos","dpos::CheckCoinStake | error | coinstake not found\n");
		return false;
	}
	
    CBlockIndex* pindexPrev = chainActive.Tip();
	assert(pindexPrev);
    int blockheight = pindexPrev->GetBlockHash() == block.hashPrevBlock ? pindexPrev->nHeight + 1 : -1;

	assert(coinstake);
	if(blockheight >= crp::GetPosStartHeight()){
		LogPrint("dpos","dpos::CheckCoinStake | info | blockheight=%d, block=%s\n",blockheight, block.GetHash().ToString());
		if(!Entrustment::GetInstance().CheckCoinStake(blockheight,*coinstake)){
			LogPrint("dpos","| dpos::CheckCoinStake | warning | Entrustment::GetInstance().CheckCoinStake() failed, blockheight=%d\n",blockheight);
			return false;
		}
	}

	return true;
}

void dpos::AppendEntrustReward(const std::map<CBitcoinAddress,CAmount>& blockRewardMap, std::vector<CTxOut>& vout)
{
	LogPrint("dpos","Entrustment::AppendEntrustReward | blockRewardMap.size=%u\n",blockRewardMap.size());
	for(std::map<CBitcoinAddress,CAmount>::const_iterator it = blockRewardMap.begin(); it != blockRewardMap.end(); it++){
		if(vout.size() >= MAX_STANDARD_VOUT_COUNT){
			LogPrint("dpos","Entrustment::AppendEntrustReward | vout.size()=%u, break\n",vout.size());
			break;
		}
		CTxOut txout(it->second,GetScriptForDestination(it->first.Get()));
		txout.SetType(CTxOut::Type::ENTRUST_REWARD);
		LogPrint("dpos","Entrustment::AppendEntrustReward | txout=%s\n",txout.ToString());
		vout.push_back(txout);
	}
}

void dpos::AppendMonthlyReward(const std::map<CBitcoinAddress,CAmount>& rewardMap, std::vector<CTxOut>& vout){
	for(std::map<CBitcoinAddress,CAmount>::const_iterator it = rewardMap.begin(); it != rewardMap.end(); it++){
		const CBitcoinAddress& addr = it->first;
		const CAmount& reward = it->second;
		CTxOut txout(reward,GetScriptForDestination(addr.Get()));
		txout.SetType(CTxOut::Type::MONTHLY_REWARD);
		LogPrint("dpos","dpos::AppendMonthlyReward | txout=%s, addr=%s\n",txout.ToString(),addr.ToString());
		vout.push_back(txout);
	}
}

std::map<CBitcoinAddress,CAmount> Entrustment::GetMonthlyRewardMap(BlockHeight blockheight) const
{
	std::map<CBitcoinAddress,CAmount> result;

	if(!crp::CoinReleasePlan::GetInstance().IsSeasonRewardHeight(blockheight))
		return result;

	std::vector<std::pair<const Agent*,CAmount>> reward_vec = GetSeasonReward(blockheight-1, blockheight);
	size_t size = reward_vec.size();
	for(int i=0; i<size; i++){
		const Agent* agent = reward_vec[i].first;
		CAmount reward = reward_vec[i].second;
		CBitcoinAddress addr = GetReceiveAddressOfAgent(agent->id);
		if(!addr.IsValid())
		{
			LogPrint("dpos","WARNING Entrustment::GetMonthlyRewardMap | invalid addr | agentid=%s, addr=%s\n",agent->id.ToString(),addr.ToString());
			continue;
		}
		result.insert(make_pair(addr,reward));
	}

	return result;
}

static crp::DistributeRule GetDistributeRule(const BlockHeight& height, const BlockHeight& forkheight)
{
	crp::DistributeRule rule(27,72);

	if(height >= forkheight)
		rule = crp::DistributeRule(50,100);
	
	return rule;
}

std::vector<std::pair<const Agent*,CAmount>> Entrustment::GetSeasonReward(BlockHeight base_height, BlockHeight new_height) const
{
	std::vector<std::pair<const Agent*,CAmount>> result;
	crp::BlockValue bv = crp::GetBlockValue(new_height);
	std::map<crp::Agentid,const Agent*> agentMap;
	std::vector<crp::Agent> agentVec;
	crp::Agent crpAgent;
	AgentMapMap::const_iterator agentMapsIt = agentMaps.find(base_height);
	if(agentMapsIt == agentMaps.end()){
		LogPrint("dpos","Entrustment::GetSeasonReward | agent map of height %d not found\n",base_height);
		return result;
	}
	const AgentMap& srcAgentMap = agentMapsIt->second;
	crp::Agentid id = 0;
	for(AgentMap::const_iterator it = srcAgentMap.begin(); it != srcAgentMap.end(); it++,id++){
		const Agent& agent = it->second;
		if(AgentIsInBlacklist(agent.id,blackListStatus,base_height))
			continue;
		agentMap[id] = &agent;
		crpAgent.id = id;
		crpAgent.blockAmount = agent.monthlyCount.blockAmount;
		if(IsSeasonRewardV2(new_height))
			crpAgent.weight = agent.monthlyCount.reward_factor;
		else
			crpAgent.weight = agent.monthlyCount.entrustAmount;
		crpAgent.lastBlockTime = agent.lastBlockTime;
		agentVec.push_back(crpAgent);
	}
	std::sort(agentVec.begin(),agentVec.end(),crp::LeftAgentMoreActive);

	CAmount boobyPrize = crp::CoinReleasePlan::GetInstance().GetPosPlan(new_height).boobyPrize;
	std::vector<std::pair<crp::Agentid,crp::CoinAmount>> distVec = crp::GetMonthlyDistribution(agentVec,bv.topFirst,bv.topSecond,GetDistributeRule(new_height,forkheight_seasonRewardTop150),boobyPrize);
	const int N = distVec.size();
	for(int i=0; i<N; i++){
		crp::Agentid crpid = distVec[i].first;
		CAmount reward = distVec[i].second;
		if(agentMap.count(crpid) == 0)
			continue;
		const Agent* agent = agentMap[crpid];
		result.push_back(std::make_pair(agent,reward));
	}
	return result;
}

CBitcoinAddress dpos::GetReceiveAddressOfAgent(const AgentID& agentid){
	CBitcoinAddress addr;
	CTransaction tx;
	if(FindConfirmedTransaction(agentid,tx)){
		CTxOut txout;
		int index = 0;
		if(tx.IsCreateAgent() && tx.GetCreateAgentOut(txout,index) && txout.IsCreateAgentType()){
			std::vector<CBitcoinAddress> addresses = txout.GetScriptAddresses();
			if(!addresses.empty())
				addr = addresses[0];
		}
	}
	return addr;
}

Entrustment& Entrustment::GetInstance(){
	static Entrustment sigleton;
	return sigleton;
}

dpos::Blacklist Entrustment::GetSeasonRewardBlacklist(BlockHeight h) const
{
	if(blackListStatus.count(h) > 0)
		return blackListStatus.at(h);
	return Blacklist();
}

int Entrustment::GetMetaBlockVersion(BlockHeight height) const
{
	return BlackAgentListAvailable(height) ? CLIENT_VERSION : 2040000; // latest or v2.4.0
}

bool Entrustment::BlackAgentListAvailable(BlockHeight height) const
{
	return (height >= forkheight_blackAgentList);
}


bool dpos::AddMetaBlock(CBlockIndex* pindex)
{
	bool success = false;
	if(!pindex){
		LogPrint("dpos","dpos::AddMetaBlock | pindex is null\n");
		return false;
	}
	CDiskBlockPos meta_block_pos;
	dpos::DposMetaBlock meta_block;
	{
		static CCriticalSection metaBlockDb;
		LOCK(metaBlockDb);
		const int metaBlockVersion = Entrustment::GetInstance().GetMetaBlockVersion(pindex->nHeight);
		if(!dpos::DposMetaBlockIndexDB::GetInstance().ReadMetaBlockIndex(pindex->GetBlockHash(), meta_block_pos)){
			CBlock block;
			if (!ReadBlockFromDisk(block, pindex)) {
				LogPrintf("error | dpos::AddMetaBlock | read block failed\n");
				return false;
			}
			if(!WriteDposMetaBlock(block,metaBlockVersion)){
				LogPrintf("error | dpos::AddMetaBlock | Write dpos meta block failed\n");
				return false;
			}
			success = dpos::DposMetaBlockIndexDB::GetInstance().ReadMetaBlockIndex(pindex->GetBlockHash(), meta_block_pos);
			assert(success);
		}
		success = dpos::DposMetaBlockIO::GetInstance().Read(meta_block_pos, meta_block, metaBlockVersion);
		assert(success);
		LogPrint("dpos","dpos::AddMetaBlock | read block#%d, meta_block_pos=%s, meta_block=%s\n",pindex->nHeight,meta_block_pos.ToString(),meta_block.ToString());
	}
	if(!Entrustment::GetInstance().AddMetaBlock(pindex->nHeight,meta_block)){
		LogPrint("dpos","dpos::AddMetaBlock | warning: AddMetaBlock() failed\n");
		return false;
	}
	return true;
}

MonthlyCount::MonthlyCount(){
	StartNewMonth();
}
void MonthlyCount::StartNewMonth(){
	blockAmount = 0;
	entrustAmount = 0;
	expect_reward = 0;
	reward_factor = 0;
}

CScript dpos::GetCreatorPubKeyOf(AgentID agentid)
{
	CScript pubKey;
	CTransaction createTx;
	CTxOut crearteTxout;
	int voutIndex = -1;
	if( dpos::FindConfirmedTransaction(agentid,createTx) && 
		createTx.GetCreateAgentOut(crearteTxout,voutIndex) )
	{
		pubKey = crearteTxout.scriptPubKey;
	}
	return pubKey;
}

vector<CBitcoinAddress> dpos::GetAddressesInScript(const CScript& script)
{
	std::vector<CBitcoinAddress> result;
	std::vector<CTxDestination> addresses;
    txnouttype type;
    int nRequired;
    if (!ExtractDestinations(script, type, addresses, nRequired))
		return result;
	for(std::vector<CTxDestination>::const_iterator it = addresses.begin(); it != addresses.end(); it++)
	{
		result.push_back(CBitcoinAddress(*it));
	}
	return result;
}

bool dpos::LoadEntrustmentCache()
{
	return true;
}

int dpos::DposMetaBlock::blackAgentListVersion = 2040100; // v2.4.1

DposMetaBlock::DposMetaBlock()
{
	// do nothing
	time = 0;
}

DposMetaBlock::DposMetaBlock(const CBlock& block)
{
	// Meta data analysis
	blockhash = block.GetHash();
	time = block.GetBlockTime();
	for(const CTransaction& tx : block.vtx)
	{
		const std::vector<CTxOut>& vout = tx.vout;
		const std::vector<CTxIn>& vin = tx.vin;
		const size_t vout_size = vout.size();
		if(tx.IsCoinStake()){
			agentid = tx.agentid;
			for(int i=0; i<vout_size; i++){
				const CTxOut& txout = vout[i];
				if(txout.IsEntrustReward() || txout.type == CTxOut::Type::MONTHLY_REWARD)
				{
					std::vector<CBitcoinAddress> addresses = txout.GetScriptAddresses();
					if(addresses.empty())
						continue;
					CBitcoinAddress user = addresses[0];

					Reward reward;
					reward.type = int(txout.IsEntrustReward() ? RewardType::ENTRUST : RewardType::SEASON);
					reward.addr = user.ToString();
					reward.amount = txout.nValue;

					reward_list.push_back(reward);
				}
			}
		}
		else if(tx.IsEntrust()){
			for(int i=0; i<vout_size; i++)
			{
				const CTxOut& txout = vout[i];
				if(txout.IsEntrusted())
				{
					std::vector<CBitcoinAddress> addresses = txout.GetScriptAddresses();
					if(addresses.empty())
						continue;
					CBitcoinAddress user = addresses[0];
					
					Entrust entrust;
					entrust.addr = user.ToString();
					entrust.type = int(EntrustType::ENTRUST);
					entrust.agentid = tx.agentid;
					entrust.amount = txout.nValue;
					entrust.txid = tx.GetHash();
					entrust.vout_i = i;
					
					entrust_list.push_back(entrust);
					break;
				}
			}
		}
		else if(tx.IsDeprive()){
			for(const CTxIn& txin : vin)
			{
				const COutPoint& outpoint = txin.prevout;
				CTxOut txout;
				if(GetConfirmedTxOut(outpoint,txout))
				{
					if(txout.IsEntrusted())
					{
						std::vector<CBitcoinAddress> addresses = txout.GetScriptAddresses();
						if(addresses.empty())
							continue;
						CBitcoinAddress user = addresses[0];
							
						Entrust entrust;
						entrust.addr = user.ToString();
						entrust.type = int(EntrustType::DEPRIVE);
						entrust.agentid = tx.agentid;
						entrust.amount = txout.nValue;
						entrust.txid = outpoint.hash;
						entrust.vout_i = outpoint.n;
						
						entrust_list.push_back(entrust);
						break;
					}
				}
			}
		}
		else if(tx.IsCreateAgent()){
			Create create;
			create.agentid = tx.GetHash();
			create.name = tx.GetAgentName();
			create_list.push_back(create);
		}
		else if(tx.IsResignAgent()){
			Resign resign;
			resign.agentid = tx.agentid;
			resign_list.push_back(resign);
		}
		else{
			for(int i=0; i<vout_size; i++){
				const CTxOut& txout = vout[i];
				if(scriptex::IsBlackAgent(txout.scriptPubKey))
				{
					AgentID agentid;
					scriptex::bytes_t signature;
					scriptex::DecodeBlackAgentFromScript(txout.scriptPubKey, agentid, signature);
					blackagent_list.push_back(agentid);
				}
			}
		}
	}
}

UniValue DposMetaBlock::ToUniValue() const
{
	UniValue root(UniValue::VOBJ);
	root.push_back(Pair("blockhash",blockhash.ToString()));
	root.push_back(Pair("agentid",agentid.ToString()));
	root.push_back(Pair("time",time));
	
	UniValue create_list_arr(UniValue::VARR);
	for(vector<Create>::const_iterator it = create_list.begin(); it != create_list.end(); it++){
		UniValue obj(UniValue::VOBJ);
		obj.push_back(Pair("agentid",it->agentid.ToString()));
		obj.push_back(Pair("name",it->name));
		create_list_arr.push_back(obj);
	}
	root.push_back(Pair("create_list",create_list_arr));
	
	UniValue resign_list_arr(UniValue::VARR);
	for(vector<Resign>::const_iterator it = resign_list.begin(); it != resign_list.end(); it++){
		UniValue obj(UniValue::VOBJ);
		obj.push_back(Pair("agentid",it->agentid.ToString()));
		resign_list_arr.push_back(obj);
	}
	root.push_back(Pair("resign_list",resign_list_arr));

	UniValue entrust_list_arr(UniValue::VARR);
	for(vector<Entrust>::const_iterator it = entrust_list.begin(); it != entrust_list.end(); it++){
		UniValue obj(UniValue::VOBJ);
		obj.push_back(Pair("addr",it->addr));
		obj.push_back(Pair("type",it->type));
		obj.push_back(Pair("agentid",it->agentid.ToString()));
		obj.push_back(Pair("amount",it->amount));
		obj.push_back(Pair("txid",it->txid.ToString()));
		obj.push_back(Pair("vout_i",it->vout_i));
		entrust_list_arr.push_back(obj);
	}
	root.push_back(Pair("entrust_list",entrust_list_arr));

	UniValue reward_list_arr(UniValue::VARR);
	for(vector<Reward>::const_iterator it = reward_list.begin(); it != reward_list.end(); it++){
		UniValue obj(UniValue::VOBJ);
		obj.push_back(Pair("addr",it->addr));
		obj.push_back(Pair("type",it->type));
		obj.push_back(Pair("amount",it->amount));
		reward_list_arr.push_back(obj);
	}
	root.push_back(Pair("reward_list",reward_list_arr));

	UniValue blackagent_list_arr(UniValue::VARR);
	for(vector<AgentID>::const_iterator it = blackagent_list.begin(); it != blackagent_list.end(); it++){
		blackagent_list_arr.push_back(it->ToString());
	}
	root.push_back(Pair("blackagent_list",blackagent_list_arr));
	return root;
}

string DposMetaBlock::ToString() const
{
	UniValue root = ToUniValue();
	return root.write();
}

DposMetaBlockIO& DposMetaBlockIO::GetInstance()
{
	static DposMetaBlockIO sigleton;
	return sigleton;
}

bool DposMetaBlockIO::LoadLastBlockPos()
{
	LOCK(cs_last_file);
	
	DposMetaBlockIndexDB& index_db = DposMetaBlockIndexDB::GetInstance();
	assert(last_block_pos.IsNull());
	bool readOk = index_db.ReadLastPos(last_block_pos);
	if(readOk)
	{
		FILE* file = OpenDposMetaBlockFile(last_block_pos);
		if(!file)
		{
			LogPrintf("DposMetaBlockIO::LoadLastBlockPos | ERROR: Open meta block file failed. last_block_pos=%s\n",last_block_pos.ToString());
			return false;
		}
		CAutoFile autofile(file, SER_DISK, CLIENT_VERSION);
		long fileTail = ftell(file);
		if(fileTail != last_block_pos.nPos)
		{
			LogPrintf("DposMetaBlockIO::LoadLastBlockPos | ERROR: last_block_pos is not match with tail of meta block file failed. last_block_pos=%s, fileTail=%ld\n",last_block_pos.ToString(),fileTail);
			return false;
		}
	}
	return readOk;
}

bool DposMetaBlockIO::Write(const DposMetaBlock& block, int version)
{
	LOCK(cs_last_file);
	
	DposMetaBlockIndexDB& index_db = DposMetaBlockIndexDB::GetInstance();
	if(index_db.ExistsBlock(block.blockhash))
		return true;
	
	unsigned int write_bytes = ::GetSerializeSize(block, SER_DISK, CLIENT_VERSION);
	if(!CheckDiskSpace(write_bytes))
		return false;

	if(last_block_pos.IsNull())
		last_block_pos = CDiskBlockPos(0,0);
	
	FILE* file = OpenDposMetaBlockFile(last_block_pos);
	if(!file){
		LogPrint("dpos","DposMetaBlockIO::Write | open last file failed | last_block_pos=%s\n",last_block_pos.ToString());
		return false;
	}
	
    long file_size = ftell(file);
    if (file_size + write_bytes > MAX_BLOCKFILE_SIZE){
		fclose(file);
		last_block_pos.nFile++;
		last_block_pos.nPos = 0;
		file = OpenDposMetaBlockFile(last_block_pos);
	}
	
	CAutoFile autofile(file, SER_DISK, version);
	autofile << block;
	if(!index_db.WriteMetaBlockIndex(block.blockhash,last_block_pos)){
		LogPrintf("ERROR: DposMetaBlockIO::Write | index_db.WriteMetaBlockIndex(%s,%s) failed\n",block.blockhash.ToString(),last_block_pos.ToString());
		return false;
	}

	const unsigned int old_pos = last_block_pos.nPos;
	const unsigned int new_pos = ftell(file);
	last_block_pos.nPos = new_pos;
	if(!index_db.WriteLastPos(last_block_pos)){
		LogPrintf("ERROR: DposMetaBlockIO::Write | index_db.WriteLastPos(%s) failed\n",last_block_pos.ToString());
		return false;
	}
	FileCommit(file);

	LogPrint("dpos","DposMetaBlockIO::Write | write %u bytes, last_block_pos.nPos: %u->%u, offset=%d, block=%s\n",write_bytes,old_pos,last_block_pos.nPos,new_pos-old_pos,block.ToString());
	
	return true;
}

bool DposMetaBlockIO::Read(CDiskBlockPos pos, DposMetaBlock& block, int version)
{
	LOCK(cs_last_file);
	
	FILE* file = OpenDposMetaBlockFile(pos);
	if(!file){
		LogPrint("dpos","DposMetaBlockIO::Read | block in pos '%s' not found\n",pos.ToString());
		return false;
	}
	CAutoFile autofile(file, SER_DISK, version);
	autofile >> block;
	return true;
}

FILE* DposMetaBlockIO::OpenDposMetaBlockFile(const CDiskBlockPos& pos, bool fReadOnly)
{
    return OpenDiskFile(pos, "dps", fReadOnly);
}

bool DposMetaBlockIndexDB::WriteMetaBlockIndex(uint256 blockhash, const CDiskBlockPos& pos)
{
	LOCK(cs_db);
	bool success = Write(std::make_pair('b', blockhash), pos);
	LogPrint("dpos","DposMetaBlockIndexDB::WriteMetaBlockIndex | success=%d, blockhash=%s, pos=%s\n",success,blockhash.ToString(),pos.ToString());
	if(!success)
	{
		LogPrint("dpos","DposMetaBlockIndexDB::WriteMetaBlockIndex | base member: pdb=%p, strFile=%s, activeTxn=%p, fReadOnly=%d\n",pdb,strFile,activeTxn,fReadOnly);
	}
	return success;
}

bool DposMetaBlockIndexDB::ReadMetaBlockIndex(uint256 blockhash, CDiskBlockPos& pos)
{
	LOCK(cs_db);
	bool success = Read(std::make_pair('b', blockhash), pos);
	LogPrint("dpos","DposMetaBlockIndexDB::ReadMetaBlockIndex | success=%d, blockhash=%s, pos=%s\n",success,blockhash.ToString(),pos.ToString());
	return success;
}

bool DposMetaBlockIndexDB::ExistsBlock(uint256 blockhash)
{
	LOCK(cs_db);
	return Exists(std::make_pair('b',blockhash));
}

bool DposMetaBlockIndexDB::WriteLastPos(const CDiskBlockPos& pos)
{
	LOCK(cs_db);
	const string key = "lastpos";
	bool success = Write(key,pos);
	LogPrint("dpos","DposMetaBlockIndexDB::WriteLastPos | success=%d, pos=%s\n",success,pos.ToString());
	return success;
}

bool DposMetaBlockIndexDB::ReadLastPos(CDiskBlockPos& pos)
{
	LOCK(cs_db);
	const string key = "lastpos";
	if(!Exists(key)){
		CDiskBlockPos lastpos;
		lastpos.nFile = 0;
		lastpos.nPos = 0;
		if(!Write(key,lastpos))
			LogPrint("dpos","write last pos failed\n");
	}
	bool success = Read(key,pos);
	LogPrint("dpos","DposMetaBlockIndexDB::ReadLastPos | pos=%s\n",pos.ToString());
	return success;
}

bool dpos::WriteDposMetaBlock(const CBlock& block, int version)
{
	DposMetaBlock meta_block(block);
	return DposMetaBlockIO::GetInstance().Write(meta_block, version);
}

DposMetaBlockIndexDB& DposMetaBlockIndexDB::GetInstance()
{
	static boost::filesystem::path filepath = GetDataDir()/"blocks"/"dpos_meta_block_index.db";
	static DposMetaBlockIndexDB singleton(filepath.string().c_str(),"cr+");
	return singleton;
}

bool DescBySeasonEntrustAmount::Compare(const Agent* a, const Agent* b) const
{
	assert(a && b);
	return a->monthlyCount.entrustAmount > b->monthlyCount.entrustAmount;
}

bool DescBySeasonRewardFactor::Compare(const Agent* a, const Agent* b) const
{
	assert(a && b);
	return a->monthlyCount.reward_factor > b->monthlyCount.reward_factor;
}

bool dpos::IsEntrustTxLocking(const CTransaction& tx, int chainHeight, uint256 blockhash)
{
	if(!mapBlockIndex.count(blockhash)) {
		return true;
	}
	const BlockHeight blockheight = mapBlockIndex[blockhash]->nHeight;
	const BlockHeight confirm = chainHeight - blockheight;
	int lockHeight = Entrustment::GetInstance().GetEntrustTxLockHeight();
	if (chainHeight >= forkheight_release_v_2_7_0 && tx.IsEntrustWithCommentTx()) {
		int lockHeight2 = Entrustment::GetInstance().GetEntrustWithCommentTxLockHeight(chainHeight);
		lockHeight = std::max(lockHeight, lockHeight2);
	}
	bool locking = (confirm < lockHeight);
	return locking;
}



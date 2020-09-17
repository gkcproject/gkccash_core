
#include "entrustment_impl.h"

#include "entrustment.h"
#include "primitives/block.h"
#include "main.h"
#include "util.h"
#include <sstream> //std::ostringstream
#include "utilmoneystr.h" //FormatMoney
#include "txdb.h"
#include "crp/CoinReleasePlan.h"

using namespace entrustment_impl;
using namespace dpos;

bool entrustment_impl::AgentIsInBlacklist(AgentID agentid, const map<BlockHeight,Blacklist>& blackListStatus, BlockHeight height)
{
	return blackListStatus.count(height) > 0 && blackListStatus.at(height).Exist(agentid);
}

void entrustment_impl::CopyBlacklistStatus(map<BlockHeight,Blacklist>& blackListStatus, BlockHeight src, BlockHeight tar, size_t saveCount)
{
	if(blackListStatus.count(tar) > 0)
		return; //'tar' height exists

	blackListStatus[tar]; // create an empty blacklist

	if(blackListStatus.count(src) > 0)
		blackListStatus[tar] = blackListStatus[src]; // copy src -> tar
	
	const BlockHeight lowerBound = tar - saveCount + 1;
	for(BlockHeight h = LowestHeightOf(blackListStatus); h < lowerBound; h++)
		blackListStatus.erase(h);
}

BlockHeight entrustment_impl::LowestHeightOf(const std::map<BlockHeight,Blacklist>& blackListMap)
{
	BlockHeight minHeight = std::numeric_limits<BlockHeight>::max();
	for(std::map<BlockHeight,Blacklist>::const_iterator it = blackListMap.begin(); it != blackListMap.end(); it++)
		minHeight = std::min(it->first,minHeight);
	return minHeight;
}

void entrustment_impl::WriteBlacklistToFile(const Blacklist& blist, std::ostream& os)
{
	std::vector<AgentID> vec;
	blist.ListAgents(vec);
	os << vec.size() << '\n';
	for(const auto& id : vec)
		os << id.ToString() << '\n';
	os << std::flush;
}

bool entrustment_impl::read_entrustment_v2(std::istream& is, AgentMapMap& agentMaps, map<BlockHeight,Blacklist>& blackListStatus)
{
	int height = 0, agentCount = 0 , blackCount = 0;
	is >> height;
	is >> agentCount;
	//std::cout << __func__ << ":height=" << height << ",agentCount=" << agentCount << std::endl;
	bool ok = read_agent_map_from_istream(is,agentCount,agentMaps[height]);
	if(!ok)
		return false;
	is >> blackCount;
	//std::cout << __func__ << ":blackCount=" << blackCount << ",peek=" << is.peek() << std::endl;
	is.ignore();
	ok = read_blacklist_from_istream(is,blackCount,blackListStatus[height]);
	return ok;
}

bool entrustment_impl::read_entrustment_v3(std::istream& is, AgentMapMap& agentMaps, map<BlockHeight,Blacklist>& blackListStatus)
{
	int height = 0, agentCount = 0 , blackCount = 0;
	is >> height;
	is >> agentCount;
	for(int i=0; i<agentCount; i++){
		Agent agent;
		is >> agent;
		agentMaps[height].insert(make_pair(agent.id,agent));
	}
	is >> blackCount;
	is.ignore();
	bool ok = read_blacklist_from_istream(is,blackCount,blackListStatus[height]);
	return ok;
}

bool entrustment_impl::read_agent_map_from_istream(std::istream& file, int count, AgentMap& agentMap)
{
	for(int i=0; i<count; i++){
		Agent agent;
		agent.UnserializeFromTextWithoutVersion(file);
		agentMap.insert(make_pair(agent.id,agent));
	}
	return true;
}

bool entrustment_impl::read_blacklist_from_istream(std::istream& file, int count, Blacklist& blist)
{
	for(int i=0; i<count; i++){

		std::string line;
		if(!std::getline(file,line))
			return false;
		AgentID id;
		id.SetHex(line);
		//std::cout << __func__ << ":line=" << line << ",id=" << id.ToString() << std::endl;
		blist.Add(id);
	}
	return true;
}

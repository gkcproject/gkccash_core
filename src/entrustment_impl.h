
#ifndef ENTRUSTMENT_IMPL_H
#define ENTRUSTMENT_IMPL_H

#include "entrustment.h"
#include <iostream>

namespace entrustment_impl
{
	bool AgentIsInBlacklist(AgentID agentid, const map<BlockHeight,dpos::Blacklist>& blackListStatus, BlockHeight height);
	void CopyBlacklistStatus(map<BlockHeight,dpos::Blacklist>& blackListStatus, BlockHeight src, BlockHeight tar, size_t saveCount);
	BlockHeight LowestHeightOf(const std::map<BlockHeight,dpos::Blacklist>& blackListMap);
	void WriteBlacklistToFile(const dpos::Blacklist& blist, std::ostream& os);
	bool read_entrustment_v2(std::istream& is, AgentMapMap& agentMaps, map<BlockHeight,dpos::Blacklist>& blackListStatus);
	bool read_entrustment_v3(std::istream& is, AgentMapMap& agentMaps, map<BlockHeight,dpos::Blacklist>& blackListStatus);
	bool read_blacklist_from_istream(std::istream& file, int count, dpos::Blacklist& blist);
	bool read_agent_map_from_istream(std::istream& file, int count, AgentMap& agentMap);
}

#endif // ~ENTRUSTMENT_IMPL_H

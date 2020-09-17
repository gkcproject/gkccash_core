// Copyright (c) 2013 The Bitcoin Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/test/unit_test.hpp>
#include "entrustment_impl.h"

using namespace entrustment_impl;
using namespace dpos;

BOOST_AUTO_TEST_SUITE(season_reward_tests)

BOOST_AUTO_TEST_CASE(test_Blacklist) {
	AgentID a1,a2,a3;
	a1.SetHex("0000000000000000000000000000000000000000000000000000000000000001");
	a2.SetHex("0000000000000000000000000000000000000000000000000000000000000002");
	a3.SetHex("0000000000000000000000000000000000000000000000000000000000000003");

	Blacklist blist;
	blist.Add(a1);
	blist.Add(a2);
	BOOST_CHECK(blist.Exist(a1)==true);
	BOOST_CHECK(blist.Exist(a3)==false);
	
	Blacklist blist2 = blist;
	blist2.Add(a3);
	BOOST_CHECK(blist2.Exist(a1)==true);
	BOOST_CHECK(blist2.Exist(a3)==true);

	std::vector<AgentID> vec;
	blist2.ListAgents(vec);
	BOOST_CHECK(vec.size()==3);
	
	blist2.Remove(a2);
	vec.clear();
	blist2.ListAgents(vec);
	BOOST_CHECK(vec.size()==2);

	blist2.Clear();
	vec.clear();
	blist2.ListAgents(vec);
	BOOST_CHECK(vec.empty());
}

BOOST_AUTO_TEST_CASE(test_AgentIsInBlacklist) {
	AgentID agentid;
	map<BlockHeight,Blacklist> blackListStatus;
	BlockHeight height;
	
	agentid.SetHex("0000000000000000000000000000000000000000000000000000000000000001");
	height = 1;
	blackListStatus[height].Add(agentid);
	
	BOOST_CHECK(AgentIsInBlacklist(agentid,blackListStatus,height));
	
	BOOST_CHECK(AgentIsInBlacklist(agentid,blackListStatus,2)==false);
	BOOST_CHECK(AgentIsInBlacklist(AgentID("0000000000000000000000000000000000000000000000000000000000000002"),blackListStatus,height)==false);
}

BOOST_AUTO_TEST_CASE(test_CopyBlacklistStatus) {
	AgentID a1,a2;
	a1.SetHex("0000000000000000000000000000000000000000000000000000000000000001");
	a2.SetHex("0000000000000000000000000000000000000000000000000000000000000002");
	
	map<BlockHeight,Blacklist> blackListStatus;
	BlockHeight src = 1;
	BlockHeight tar = 2;
	size_t saveCount = 2;
	blackListStatus[0].Add(a1);
	blackListStatus[0].Add(a2);
	blackListStatus[1].Add(a1);
	blackListStatus[1].Add(a2);
	BOOST_CHECK(blackListStatus.count(tar) == 0);
	CopyBlacklistStatus(blackListStatus,src,tar,saveCount);
	BOOST_CHECK(blackListStatus.count(tar) > 0);
	if(blackListStatus.count(tar) > 0){
		const Blacklist& blist = blackListStatus.at(tar);
		BOOST_CHECK(blist.Exist(a1));
		BOOST_CHECK(blist.Exist(a2));
	}
	BOOST_CHECK(blackListStatus.size() == saveCount);
	BOOST_CHECK(blackListStatus.count(0) == 0);
	BOOST_CHECK(blackListStatus.count(1) > 0);
	BOOST_CHECK(blackListStatus.count(2) > 0);
}

BOOST_AUTO_TEST_CASE(test_LowestHeightOf)
{
	std::map<BlockHeight,Blacklist> bMap;
	const BlockHeight infinite = std::numeric_limits<BlockHeight>::max();
	BOOST_CHECK(LowestHeightOf(bMap) == infinite);

	bMap[0];
	bMap[2];
	bMap[1];
	BOOST_CHECK(LowestHeightOf(bMap) == 0);
}

BOOST_AUTO_TEST_CASE(test_WriteBlacklistToFile)
{
	AgentID a1,a2;
	a1.SetHex("0000000000000000000000000000000000000000000000000000000000000001");
	a2.SetHex("0000000000000000000000000000000000000000000000000000000000000002");
	Blacklist blist;
	std::ostringstream os;
	blist.Add(a1);
	blist.Add(a2);
	WriteBlacklistToFile(blist,os);
	std::string target = "2\n0000000000000000000000000000000000000000000000000000000000000001\n0000000000000000000000000000000000000000000000000000000000000002\n";
	BOOST_CHECK(os.str()==target);
}

BOOST_AUTO_TEST_CASE(test_read_entrustment_v101)
{
	AgentID a1,a2;
	a1.SetHex("0000000000000000000000000000000000000000000000000000000000000001");
	a2.SetHex("0000000000000000000000000000000000000000000000000000000000000002");
	std::istringstream is(
		"0\n" // height
		"0\n" // agentCount
		"2\n" // blackCount
		"0000000000000000000000000000000000000000000000000000000000000001\n" // blackAgent
		"0000000000000000000000000000000000000000000000000000000000000002\n" // blackAgent
		);
	AgentMapMap agentMaps;
	map<BlockHeight,Blacklist> blackListStatus;
	BOOST_CHECK(read_entrustment_v2(is,agentMaps,blackListStatus));
	BOOST_CHECK(blackListStatus.size() == 1);
	BOOST_CHECK(blackListStatus.count(0) > 0);
	if(blackListStatus.count(0) > 0)
	{
		const Blacklist& blist = blackListStatus.at(0);
		BOOST_CHECK(blist.Exist(a1));
		BOOST_CHECK(blist.Exist(a2));
	}
}

BOOST_AUTO_TEST_SUITE_END()

// Copyright (c) 2013 The Bitcoin Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/test/unit_test.hpp>
#include "../crp/CoinReleasePlan.h"

using namespace std;
using namespace crp;

BOOST_AUTO_TEST_SUITE(crp_tests)

BOOST_AUTO_TEST_CASE(test_HeightRange)
{
	HeightRange r;
	BOOST_CHECK(r.Begin()==0);
	BOOST_CHECK(r.Back()==HeightRange::InfiniteHeight());
	BOOST_CHECK(r.End()==HeightRange::InfiniteHeight());
	BOOST_CHECK(r.Exists(-1)==false);
	BOOST_CHECK(r.Exists(0));
	BOOST_CHECK(r.Exists(1));
	BOOST_CHECK(r.Exists(std::numeric_limits<Height>::max()-1));
}

BOOST_AUTO_TEST_CASE(test_HeightRange_range)
{
	HeightRange r;
	r.begin = 1;
	r.range = 2;
	BOOST_CHECK(r.Begin()==1);
	BOOST_CHECK(r.Back()==2);
	BOOST_CHECK(r.End()==3);
	BOOST_CHECK(r.Exists(0)==false);
	BOOST_CHECK(r.Exists(1));
	BOOST_CHECK(r.Exists(2));
	BOOST_CHECK(r.Exists(3)==false);
}

BOOST_AUTO_TEST_CASE(test_DistributeRule) {
	std::vector<Agent> blockCountVec;
	for(int i=0; i<200; i++){
		Agent a;
		a.id = i;
		a.weight = 1;
		a.blockAmount = 1;
		a.lastBlockTime = 1;
		blockCountVec.push_back(a);
	}
	CoinAmount topFirstTotalAmount = 10000;
	CoinAmount topSecondTotalAmount = 10000;

	std::vector<std::pair<Agentid,CoinAmount>> result;
	
	result = GetMonthlyDistribution(blockCountVec,topFirstTotalAmount,topSecondTotalAmount,DistributeRule(27,72),0);
	BOOST_CHECK(result.size() == 99);

	result = GetMonthlyDistribution(blockCountVec,topFirstTotalAmount,topSecondTotalAmount,DistributeRule(50,100),0);
	BOOST_CHECK(result.size() == 150);
}

BOOST_AUTO_TEST_CASE(test_GetMonthlyDistribution) {
	std::vector<Agent> blockCountVec;
	for(int i=0; i<5; i++){
		Agent a;
		a.id = i;
		a.weight = 1;
		a.blockAmount = 1;
		a.lastBlockTime = 1;
		blockCountVec.push_back(a);
	}
	CoinAmount topFirstTotalAmount = 10000;
	CoinAmount topSecondTotalAmount = 10000;

	std::vector<std::pair<Agentid,CoinAmount>> result;
	result = GetMonthlyDistribution(blockCountVec,topFirstTotalAmount,topSecondTotalAmount,DistributeRule(2,2),0);
	BOOST_CHECK(result.size() == 4);
	BOOST_CHECK(result[0].second == 5000);
	BOOST_CHECK(result[1].second == 5000);
	BOOST_CHECK(result[2].second == 5000);
	BOOST_CHECK(result[3].second == 5000);

	result = GetMonthlyDistribution(blockCountVec,topFirstTotalAmount,topSecondTotalAmount,DistributeRule(2,4),0);
	BOOST_CHECK(result.size() == 5);
	BOOST_CHECK(result[0].second == 5000);
	BOOST_CHECK(result[1].second == 5000);
	BOOST_CHECK(result[2].second == 3334);
	BOOST_CHECK(result[3].second == 3333);
	BOOST_CHECK(result[4].second == 3333);
}

BOOST_AUTO_TEST_CASE(test_DistributeRule_TotalCount) {
	int n1 = 50, n2 = 100;
	DistributeRule rule(n1,n2);
	BOOST_CHECK(rule.TotalCount() == (n1+n2));
}


BOOST_AUTO_TEST_CASE(PosPlanV3) {
	Height forkheight = 676481;
	
	BlockValue bv;

	bv = GetBlockValue(forkheight-1);
 	BOOST_CHECK_EQUAL(bv.miner,         1.20 *COIN / 2);
 	BOOST_CHECK_EQUAL(bv.fund,          0.35 *COIN / 2);
 	BOOST_CHECK_EQUAL(bv.entrust,       6.60 *COIN / 2);
 	BOOST_CHECK_EQUAL(bv.masternode,    0.48 *COIN / 2);
//	BOOST_CHECK_EQUAL(bv.topFirst,  72800.00 *COIN / 2);
//	BOOST_CHECK_EQUAL(bv.topSecond, 72800.00 *COIN / 2);

	bv = GetBlockValue(forkheight);
 	BOOST_CHECK_EQUAL(bv.miner,         0.48 *COIN / 2);
 	BOOST_CHECK_EQUAL(bv.entrust,       5.35 *COIN / 2);
 	BOOST_CHECK_EQUAL(bv.fund,          4.16 *COIN / 2);
 	BOOST_CHECK_EQUAL(bv.masternode,    0.24 *COIN / 2);
//	BOOST_CHECK_EQUAL(bv.topFirst,  37160.00 *COIN / 2);
//	BOOST_CHECK_EQUAL(bv.topSecond, 37160.00 *COIN / 2);

	bv = GetBlockValue(forkheight+1);
 	BOOST_CHECK_EQUAL(bv.miner,         0.48 *COIN / 2);
 	BOOST_CHECK_EQUAL(bv.entrust,       5.35 *COIN / 2);
 	BOOST_CHECK_EQUAL(bv.fund,          4.16 *COIN / 2);
 	BOOST_CHECK_EQUAL(bv.masternode,    0.24 *COIN / 2);
//	BOOST_CHECK_EQUAL(bv.topFirst,  37160.00 *COIN / 2);
//	BOOST_CHECK_EQUAL(bv.topSecond, 37160.00 *COIN / 2);
}

BOOST_AUTO_TEST_SUITE_END()

// Copyright (c) 2013 The Bitcoin Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/test/unit_test.hpp>
#include "entrustment.h"
#include <sstream>
#include <string>

using namespace dpos;
using namespace std;

BOOST_AUTO_TEST_SUITE(entrustment_tests)

BOOST_AUTO_TEST_CASE(test_UserStatistics_operator) {
	
	UserStatistics u;
	u.version = 1;
	u.entrustAmount = 2;
	u.totalDealDivideReward = 3;
	u.totalDividendReward = 4;
	u.joinHeight = 5;

	ostringstream os;
	os << u;
	string s = os.str();
	BOOST_CHECK(!s.empty());
	
	istringstream is(s);
	UserStatistics tmp;
	is >> tmp;
	BOOST_CHECK(tmp.version == u.version);
	BOOST_CHECK(tmp.joinHeight == u.joinHeight);
	BOOST_CHECK(tmp.entrustAmount == u.entrustAmount);
	BOOST_CHECK(tmp.totalDealDivideReward == u.totalDealDivideReward);
	BOOST_CHECK(tmp.totalDividendReward == u.totalDividendReward);
}

BOOST_AUTO_TEST_SUITE_END()

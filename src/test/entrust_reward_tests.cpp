// Copyright (c) 2013 The Bitcoin Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/test/unit_test.hpp>
#include "entrustment.h"

BOOST_AUTO_TEST_SUITE(entrust_reward_tests)

BOOST_AUTO_TEST_CASE(test_activeuser) {
	Entrustment& ent = Entrustment::GetInstance();
	BlockHeight forkheight = ent.forkheight_halfMinDivideReward;
	
	BOOST_CHECK(ent.GetMinDivideReward(forkheight-1) == ent.GetMinDivideRewardV1());
	BOOST_CHECK(ent.GetMinDivideReward(forkheight) == ent.GetMinDivideRewardV2());
	BOOST_CHECK(ent.GetMinDivideReward(forkheight+1) == ent.GetMinDivideRewardV2());

	CAmount localMinDivideReward = ent.GetMinDivideReward(forkheight);
	
	dpos::UserStatistics user;
	user.entrustAmount = 1;
	BOOST_CHECK(ent.IsActiveUser(user,forkheight));
	
	user.entrustAmount = 0;
	user.totalDividendReward = 0;
	user.totalDealDivideReward = user.totalDividendReward + localMinDivideReward + 1;
	BOOST_CHECK(user.totalDealDivideReward - user.totalDividendReward > localMinDivideReward);
	BOOST_CHECK(ent.IsActiveUser(user,forkheight));
	
	user.totalDealDivideReward = user.totalDividendReward + localMinDivideReward;
	BOOST_CHECK(user.totalDealDivideReward - user.totalDividendReward == localMinDivideReward);
	BOOST_CHECK(ent.IsActiveUser(user,forkheight));

	user.totalDealDivideReward = user.totalDividendReward + localMinDivideReward - 1;
	BOOST_CHECK(user.totalDealDivideReward - user.totalDividendReward < localMinDivideReward);
	BOOST_CHECK(!ent.IsActiveUser(user,forkheight));
}

BOOST_AUTO_TEST_SUITE_END()

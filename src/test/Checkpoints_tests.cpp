// Copyright (c) 2011-2013 The Bitcoin Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

//
// Unit tests for block-chain checkpoints
//

#include "checkpoints.h"

#include "uint256.h"

#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_SUITE(Checkpoints_tests)

BOOST_AUTO_TEST_CASE(sanity)
{
    uint256 p60004 = uint256("0xd925388c39bb87db186e7d6ec5619ffa38e8bc34825dabf521acd6ebf10e2a63");
    uint256 p60006 = uint256("0x8addcb2ddb6c141f61e617746a7b981f16bc06e533ac6d32ae7eee9d58298aaf");
    BOOST_CHECK(Checkpoints::CheckBlock(60004, p60004));
    BOOST_CHECK(Checkpoints::CheckBlock(60006, p60006));


    // Wrong hashes at checkpoints should fail:
    BOOST_CHECK(!Checkpoints::CheckBlock(60004, p60006));
    BOOST_CHECK(!Checkpoints::CheckBlock(60006, p60004));

    // ... but any hash not at a checkpoint should succeed:
    BOOST_CHECK(Checkpoints::CheckBlock(60004-1, p60006));
    BOOST_CHECK(Checkpoints::CheckBlock(60006+1, p60004));

    BOOST_CHECK(Checkpoints::GetTotalBlocksEstimate() >= 60006);
}

BOOST_AUTO_TEST_SUITE_END()

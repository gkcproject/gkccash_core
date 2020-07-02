// Copyright (c) 2012-2013 The Bitcoin Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "scriptex.h"

#include <boost/test/unit_test.hpp>

#include "scriptex.h"
#include "script/script.h"

bool operator==(const scriptex::bytes_t& a, const scriptex::bytes_t& b)
{
	if(a.size() != b.size())
		return false;
	for(int i=0; i<a.size(); i++)
		if(a[i] != b[i])
			return false;
	return true;
}

BOOST_AUTO_TEST_SUITE(scriptex_tests)

BOOST_AUTO_TEST_CASE(test_IsBlackAgent)
{
    CScript script;
	AgentID agent("0000000000000000000000000000000000000000000000000000000000000001");
	script = scriptex::EncodeBlackAgentToScript(agent);
	BOOST_CHECK(scriptex::IsBlackAgent(script));
}

BOOST_AUTO_TEST_CASE(test_DecodeBlackAgentFromScript)
{
    CScript script;
	AgentID agent("0000000000000000000000000000000000000000000000000000000000000001");
	script = scriptex::EncodeBlackAgentToScript(agent);
	scriptex::bytes_t signature;
	signature.push_back(0x01);
	signature.push_back(0x02);
	script << signature;

	AgentID id;
	scriptex::bytes_t sig;
	scriptex::DecodeBlackAgentFromScript(script,id,sig);
	BOOST_CHECK(id == agent);
	BOOST_CHECK(sig == signature);
}


BOOST_AUTO_TEST_SUITE_END()

// Copyright (c) 2013 The Bitcoin Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/test/unit_test.hpp>
#include "rpctoken_impl.h"
#include <string>

using namespace std;
using namespace rpctoken_impl;

BOOST_AUTO_TEST_SUITE(rpctoken_tests)

BOOST_AUTO_TEST_CASE(test_uint256)
{
	uint256 u("01f4");
	BOOST_CHECK(u.Get64()==500);
}

BOOST_AUTO_TEST_CASE(test_IsTokenid)
{
	BOOST_CHECK(IsTokenid("9e27784a3d83ddc0c51240c4b257dba9b38f4241")==true);
	BOOST_CHECK(IsTokenid("")==false);
	BOOST_CHECK(IsTokenid("abcde")==false);
	BOOST_CHECK(IsTokenid("9e27784a3d83ddc0c51240c4b257dba9b38f42419e27784a3d83ddc0c51240c4b257dba9b38f4241")==false);
	BOOST_CHECK(IsTokenid("xe27784a3d83ddc0c51240c4b257dba9b38f4241")==false);
}

BOOST_AUTO_TEST_CASE(test_IsTokenAccount)
{
	BOOST_CHECK(IsTokenAccount("GUohQVm75Qkp5So9kHtbCQJkvmcpH9X9Wd")==true);
	BOOST_CHECK(IsTokenAccount("")==false);
	BOOST_CHECK(IsTokenAccount("abcde")==false);
	BOOST_CHECK(IsTokenAccount("9e27784a3d83ddc0c51240c4b257dba9b38f42419e27784a3d83ddc0c51240c4b257dba9b38f4241")==false);
	BOOST_CHECK(IsTokenAccount("aUohQVm75Qkp5So9kHtbCQJkvmcpH9X9Wd")==false);
	BOOST_CHECK(IsTokenAccount("16RBw3VejaVRFmQQ856JZovfsqBLHZCiwc")==false); // BTC PUBKEY ADDRESS
	BOOST_CHECK(IsTokenAccount("31hrvWcjjsKxNKKSFNX2Kw7UhYfrgjDzSr")==false); // BTC SCRIPT ADDRESS
	BOOST_CHECK(IsTokenAccount("GUohQVm75Qkp5So9kHtbCQJkvmcpH9X9Wa")==false);
}

BOOST_AUTO_TEST_CASE(test_IsHexTokenAccount)
{
	BOOST_CHECK(IsHexTokenAccount("07bec2a4249f0e9551ea37a17c981feeeabf29d3")==true);
	BOOST_CHECK(IsHexTokenAccount("")==false);
	BOOST_CHECK(IsHexTokenAccount("abcde")==false);
	BOOST_CHECK(IsHexTokenAccount("7bec2a4249f0e9551ea37a17c981feeeabf29d3")==false);
	BOOST_CHECK(IsHexTokenAccount("07bec2a4249f0e9551ea37a17c981feeeabf29d3abcde")==false);
	BOOST_CHECK(IsHexTokenAccount("GUohQVm75Qkp5So9kHtbCQJkvmcpH9X9Wd")==false);
}

BOOST_AUTO_TEST_CASE(test_GetHexTokenAccount)
{
	BOOST_CHECK(GetHexTokenAccount("GJYsfjF7FxB3CFinGwPunfRaUGLpsCYzXo")==string("07bec2a4249f0e9551ea37a17c981feeeabf29d3"));
	BOOST_CHECK(GetHexTokenAccount("GUohQVm75Qkp5So9kHtbCQJkvmcpH9X9Wd")==string("783db0d65a2c145e8a79c8a588db14b03bdff59f"));
}

BOOST_AUTO_TEST_CASE(test_GetTokenAccount)
{
	BOOST_CHECK(GetTokenAccount("07bec2a4249f0e9551ea37a17c981feeeabf29d3")==CBitcoinAddress("GJYsfjF7FxB3CFinGwPunfRaUGLpsCYzXo"));
	BOOST_CHECK(GetTokenAccount("783db0d65a2c145e8a79c8a588db14b03bdff59f")==CBitcoinAddress("GUohQVm75Qkp5So9kHtbCQJkvmcpH9X9Wd"));
}

BOOST_AUTO_TEST_CASE(test_CreateContractCallData)
{
	string function = "a9059cbb";
	string v1 = "ef3b5a574042fdd935aa6099f5019445928d3c78";
	string v2 = "1";
	vector<string> params;
	params.push_back(v1);
	params.push_back(v2);
	string target = "a9059cbb000000000000000000000000ef3b5a574042fdd935aa6099f5019445928d3c780000000000000000000000000000000000000000000000000000000000000001";
	BOOST_CHECK(CreateContractCallData(function,params)==target);
}

BOOST_AUTO_TEST_CASE(test_ParseContractDataStack)
{
	bytes_t data = ParseHex
		(
		"608060aaaa3d0029"
		"0000000000000000000000000000000000000000000000000000000000000080"
		"00000000000000000000000000000000000000000000000000000000000000c0"
		"0000000000000000000000000000000000000000000000000000000005f5e100"
		"0000000000000000000000000000000000000000000000000000000000000004"
		"0000000000000000000000000000000000000000000000000000000000000006"
		"4254313334350000000000000000000000000000000000000000000000000000"
		"0000000000000000000000000000000000000000000000000000000000000002"
		"4254000000000000000000000000000000000000000000000000000000000000"
		);
	
	bytes_t targets[] =
	{
		bytes_t(ParseHex("608060aaaa3d0029")),
		bytes_t(ParseHex("0000000000000000000000000000000000000000000000000000000000000080")),
		bytes_t(ParseHex("00000000000000000000000000000000000000000000000000000000000000c0")),
		bytes_t(ParseHex("0000000000000000000000000000000000000000000000000000000005f5e100")),
		bytes_t(ParseHex("0000000000000000000000000000000000000000000000000000000000000004")),
		bytes_t(ParseHex("0000000000000000000000000000000000000000000000000000000000000006")),
		bytes_t(ParseHex("4254313334350000000000000000000000000000000000000000000000000000")),
		bytes_t(ParseHex("0000000000000000000000000000000000000000000000000000000000000002")),
		bytes_t(ParseHex("4254000000000000000000000000000000000000000000000000000000000000")),
	};
	int n = 9;
	stack<bytes_t> dataStack;
	ParseContractDataStack(data,n,dataStack);
	for(int i=0; i<n; i++)
	{
		bytes_t e = dataStack.top();
		BOOST_CHECK(e == targets[i]);
		dataStack.pop();
	}
	BOOST_CHECK(dataStack.empty());
}


BOOST_AUTO_TEST_SUITE_END()

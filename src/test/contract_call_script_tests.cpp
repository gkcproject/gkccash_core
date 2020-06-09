// Copyright (c) 2013 The Bitcoin Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/test/unit_test.hpp>
#include "script/script.h"
#include "univalue.h"
#include "scriptex.h"

void ScriptPubKeyToJSON(const CScript& scriptPubKey, UniValue& out, bool fIncludeHex);

BOOST_AUTO_TEST_SUITE(contract_call_script_tests)


BOOST_AUTO_TEST_CASE(test_ScriptPubKeyToJSON) {
	int VMversion = 4;
	int gasLimit = 250000;
	int gasPrice = 40;
	valtype datahex = util::HexStringToBytes("a9059cbb0000000000000000000000006d1c32db5235887cb3c7641a5a5f2f1ce93480dc0000000000000000000000000000000000000000000000000000000000000001");
	valtype contractaddress = util::HexStringToBytes("9a33fa6383255311f08d97d32a1b5289c03cc550");
	valtype senderPubKey = util::HexStringToBytes("032ca9f848330d669cc255052ae3c5eccc7ca6812f79edd340d0f869452c175b02");
	valtype senderSignature = util::HexStringToBytes("30450221009b0d0abd40a70aa9c5e4eaeb554d13df7ac8e0fafe34a331cd4d12c396342bdc022065ded302dd4b87403c4da5b7cdc96450e6e4fe4c26aff0b229d37f04bad48a2d");
	opcodetype opcode = OP_CALL;
	CScript script = CScript() 
		<< CScriptNum(VMversion) << CScriptNum(gasLimit) << CScriptNum(gasPrice) 
		<< datahex << contractaddress
		<< senderPubKey << senderSignature
		<< opcode;
	
	UniValue out(UniValue::VOBJ);
    ScriptPubKeyToJSON(script,out,false);
	BOOST_CHECK(out["type"].isStr());
	BOOST_CHECK(out["sender"].isStr());
	BOOST_CHECK(out["type"].get_str()==std::string("call2"));
	BOOST_CHECK(out["sender"].get_str()==std::string("GX3S76PquZsG2zJAV9iq43wsC42DmLGPmM"));
}

BOOST_AUTO_TEST_SUITE_END()

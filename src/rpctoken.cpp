#include "rpctoken.h"

#include "rpcserver.h" // gethexaddress, callcontract, HelpExample
#include "base58.h" // CBitcoinAddress
#include "utilstrencodings.h" // IsHex
#include "contractcomponent.h" // AddressInUse
#include <vector>
#include "init.h" // pwalletMain
#include "wallet.h" // pwalletMain->GetKey()
#include "main.h" // chainActive
#include <stack>
#include "txmempool.h" // CCoinsViewMemPool
#include "rpcrawtransaction.h"
#include <algorithm>    // std::reverse
#include "scriptex.h" // util::BytesToHexString

#include "rpctoken_impl.h"

using namespace rpctoken_impl;
using std::string;
using std::vector;

// get token balance
UniValue gettokenbalance(const UniValue& params, bool fHelp)
{
	// TEST CASE:
	// ./gkc-cli gettokenbalance 05bf1b2d49bf2eeb61c7b1d36a328fbd9c4809cf GUohQVm75Qkp5So9kHtbCQJkvmcpH9X9Wd
	// 999898780000
	// ./gkc-cli gettokenbalance 05bf1b2d49bf2eeb61c7b1d36a328fbd9c4809c  GUohQVm75Qkp5So9kHtbCQJkvmcpH9X9Wd
	// error: {"code":-5,"message":"Invalid tokenid"}
	// ./gkc-cli gettokenbalance 05bf1b2d49bf2eeb61c7b1d36a328fbd9c4809cf GUohQVm75Qkp5So9kHtbCQJkvmcpH9X9W
	// error: {"code":-5,"message":"Invalid account"}
	// ./gkc-cli gettokenbalance 05bf1b2d49bf2eeb61c7b1d36a328fbd9c4809ca GUohQVm75Qkp5So9kHtbCQJkvmcpH9X9Wd
	// error: {"code":-5,"message":"Token not found"}
	// ./gkc-cli gettokenbalance 05bf1b2d49bf2eeb61c7b1d36a328fbd9c4809cf GUohQVm75Qkp5So9kHtbCQJkvmcpH9X9Wa
	// error: {"code":-5,"message":"Invalid account"}
	// 
	if(fHelp || params.size() < 2)
		throw std::runtime_error(
			"gettokenbalance <tokenid> <account>\n"
			"\n"
			"Arguments:\n"
			"1. \"tokenid\"  (string, required)  ERC20 contract address\n"
			"2. \"account\"  (string, required)  token account address, same as gkc address\n"
			"\n"
			"Result:\n"
			"   balance  (int64)\n"
			"\nExamples:\n"
            + HelpExample("gettokenbalance", "05bf1b2d49bf2eeb61c7b1d36a328fbd9c4809cf GUohQVm75Qkp5So9kHtbCQJkvmcpH9X9Wd")
			);

    string tokenid = params[0].get_str();
    string account = params[1].get_str();
	
	CheckTokenid(tokenid);
	CheckTokenAccount(account);
	
	const string& contractaddress = tokenid;
	string hexaccount = GetHexTokenAccount(account);
	string data = CreateContractCallData(FUNC_CODE_balanceOf,hexaccount);
	string output =CallContractAndGetOutput(contractaddress,data);
	
	uint256 bignum(output);
	return bignum.Get64();
}

// transfer token
UniValue transfertoken(const UniValue& params, bool fHelp)
{
	if(fHelp || params.size() < 4)
		throw std::runtime_error(
			"transfertoken <tokenid> <from> <to> <amount>\n"
			"\n"
			"Arguments:\n"
			"1. \"tokenid\"  (string, required)  ERC20 contract address\n"
			"2. \"from\"	 (string, required)  Token account address for sending, this account must be in the current wallet.\n"
			"3. \"to\"	     (string, required)  Token account address for receiving\n"
			"4. \"amount\"   (uint, required)  amount of token to send\n"
			"\n"
			"Result:\n"
			"   txid  (string)\n"
			"\nExamples:\n"
            + HelpExample("transfertoken", "9e27784a3d83ddc0c51240c4b257dba9b38f4241 GX5bpe3Z3QCGpQQAKaQpwpv7P4MmVFCD8F Gfes1tLf9KthaWWSXA321Gdinaj8VtYcQL 1")
			);
	
	string tokenid = params[0].get_str();
	string fromAccount = params[1].get_str();
	string toAccount = params[2].get_str();
	CAmount amount = params[3].get_int64();

	CheckTokenid(tokenid);
	CheckTokenAccount(fromAccount);
	CheckTokenAccount(toAccount);
	CheckAmount(amount);

    CBitcoinAddress address(fromAccount);
    CKeyID keyID;
    CKey vchSecret;
    if (!address.GetKeyID(keyID) || !pwalletMain || !pwalletMain->GetKey(keyID, vchSecret))
        throw JSONRPCError(RPC_WALLET_ERROR, "Token account " + fromAccount + " authorization failed");

	const string& contractaddress = tokenid;
	
	string hexToAccount = GetHexTokenAccount(toAccount);
	string amountStr = strprintf("%llx",(uint64_t)amount);
	vector<string> dataVec;
	dataVec.push_back(hexToAccount);
	dataVec.push_back(amountStr);
	string data = CreateContractCallData(FUNC_CODE_transfer,dataVec);

	return SendtoContractAndGetTxid(contractaddress,data,fromAccount);
}

// list token transactions of
UniValue listtokentransactions(const UniValue& params, bool fHelp)
{
	if(fHelp || params.size() < 2)
		throw std::runtime_error(
			"listtokentransactions <first> <last> (tokenid)\n"
			"\n"
			"Arguments:\n"
			"1. \"first\"	 (string, required)  The first height of block to query\n"
			"2. \"last\"	 (string, required)  The last height of block to query\n"
			"3. \"tokenid\"  (string, optional)  ERC20 contract address\n"
			"\n"
			"Result:\n"
			"[\n"
			"  {\n"
			"	 \"type\": \"create\",\n"
			"	 \"block\": {\n"
			"	   \"height\": 172400,\n"
			"	   \"hash\": \"7ddcbb53c60920c99ea0076f339310cb7fc0ff44af14dd7351d8e5f9b61c4748\",\n"
			"	   \"time\": 1592459282\n"
			"	 },\n"
			"	 \"output\": {\n"
			"	   \"txid\": \"b9376700389f6b9bf4253823145df7d629ab94bcd1a843506455ef9c420f0cfe\",\n"
			"	   \"vout\": 1\n"
			"	 },\n"
			"	 \"tokenid\": \"9e27784a3d83ddc0c51240c4b257dba9b38f4241\",\n"
			"	 \"from\": \"\",\n"
			"	 \"to\": \"GX5bpe3Z3QCGpQQAKaQpwpv7P4MmVFCD8F\",\n"
			"	 \"amount\": 1000000000000\n"
			"  },\n"
			"  {\n"
			"	 \"type\": \"transfer\",\n"
			"	 \"block\": {\n"
			"	   \"height\": 172418,\n"
			"	   \"hash\": \"59252b5a3259bf112ca6599c3c1d36c039a9dfa8d6b8587667f80d00b4d42dd6\",\n"
			"	   \"time\": 1592460357\n"
			"	 },\n"
			"	 \"output\": {\n"
			"	   \"txid\": \"98646bca3f443e3a1a07aadefa83ef4339a5e5c98143723cd63e3cfd42930e26\",\n"
			"	   \"vout\": 1\n"
			"	 },\n"
			"	 \"tokenid\": \"9e27784a3d83ddc0c51240c4b257dba9b38f4241\",\n"
			"	 \"from\": \"GX5bpe3Z3QCGpQQAKaQpwpv7P4MmVFCD8F\",\n"
			"	 \"to\": \"Gfes1tLf9KthaWWSXA321Gdinaj8VtYcQL\",\n"
			"	 \"amount\": 10000\n"
			"  }\n"
			"]\n"
			"\n"

			"\nExamples:\n"
            + HelpExample("listtokentransactions", "172400 172420 9e27784a3d83ddc0c51240c4b257dba9b38f4241")
			);

	BlockHeight first = params[0].get_int();
	BlockHeight last = params[1].get_int();
	string tokenid;
	if(params.size() >= 3)
		tokenid = params[2].get_str();
	
	CheckBlockHeight(first);
	CheckBlockHeight(last);
	if(!tokenid.empty())
		CheckTokenid(tokenid);
	if(last < first)
		throw std::runtime_error("<last> must be greater than or equal to <first>");

	UniValue result(UniValue::VARR);
	for(BlockHeight h = first; h <= last; h++)
	{
		assert(h >= 0);
		if(h < 0 || chainActive.Height() < h)
			continue;
		CBlockIndex* pblockindex = chainActive[h];
		if(!pblockindex)
			continue;
		//printf("%s:%s:%d|height=%d\n",__func__,__FILE__,__LINE__,pblockindex->nHeight);
		CBlock block;
		if (!ReadBlockFromDisk(block, pblockindex))
			continue;
		for(const CTransaction& tx : block.vtx)
		{
			for(int i=0; i<tx.vout.size(); i++)
			{
				const CTxOut& txout = tx.vout[i];
				if(!IsTokenTransfer(txout))
					continue;
				try
				{
					listtokentransactions_entry_t entry = listtokentransactions_entry_t::create(pblockindex,tx,i);
					if(tokenid.empty() || entry.tokenid == tokenid)
						result.push_back(entry.ToUniValue());
				}
				catch(std::runtime_error& e)
				{
					LogPrintf("%s:%s:%d|runtime_error=%s, height=%d,txid=%d,vout=%d\n",__func__,__FILE__,__LINE__,e.what(),pblockindex->nHeight,tx.GetHash().ToString(),i);
				}
			}
		}
	}
	
	//printf("%s:%s:%d|params=%s\n",__func__,__FILE__,__LINE__,params.write().c_str());
	return result;
}

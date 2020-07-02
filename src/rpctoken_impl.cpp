#include "rpctoken_impl.h"

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

using std::string;
using std::vector;

namespace rpctoken_impl
{
	const int U160_BYTE_LEN = 20;
	const int U256_BYTE_LEN = 32;
	
	const int H160_STR_LEN = U160_BYTE_LEN * 2;
	const int U256_STR_LEN = U256_BYTE_LEN * 2;

	const string FUNC_CODE_balanceOf = "70a08231";
	const string FUNC_CODE_transfer = "a9059cbb";
	
	bool IsTokenid(const string& str)
	{
		return (IsHex(str) && str.size() == H160_STR_LEN);
	}

	bool IsTokenAccount(const string& str)
	{
		return CBitcoinAddress(str).IsGKC();
	}

	bool IsHexTokenAccount(const string& str)
	{
		return IsHex(str) && str.size() == H160_STR_LEN;
	}

	void CheckTokenid(const string& tokenid)
	{
		if(!IsTokenid(tokenid))
			throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid tokenid");
		if (!AddressInUse(tokenid))
			throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Token not found");
	}

	void CheckBlockHeight(BlockHeight h)
	{
		if(h < 0)
			throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid block height");
	}

	void CheckTokenAccount(const string& account)
	{
		if(!IsTokenAccount(account))
			throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid account");
	}

	void CheckAmount(const CAmount& amount)
	{
		if(amount < 0)
			throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid amount");
	}

	string GetHexTokenAccount(const string& tokenAccount)
	{
		// ./gkc-cli gethexaddress GUohQVm75Qkp5So9kHtbCQJkvmcpH9X9Wd
		// 783db0d65a2c145e8a79c8a588db14b03bdff59f
		//
		UniValue params(UniValue::VARR);
		params.push_back(tokenAccount);
		UniValue result = gethexaddress(params,false);
		string hexaccount = result.get_str();
		if(hexaccount.length() != H160_STR_LEN)
			throw std::runtime_error("invalid hexaccount");
		return hexaccount;
	}

	TokenAccount GetTokenAccount(const string& hexTokenAccount)
	{
		// ./gkc-cli fromhexaddress 783db0d65a2c145e8a79c8a588db14b03bdff59f
		// GUohQVm75Qkp5So9kHtbCQJkvmcpH9X9Wd
		//
		UniValue params(UniValue::VARR);
		params.push_back(hexTokenAccount);
		UniValue result = fromhexaddress(params,false);
		string tokenAccountStr = result.get_str();
		CBitcoinAddress tokenAccount(tokenAccountStr);
		if(!tokenAccount.IsGKC())
			throw std::runtime_error("invalid token account");
		return tokenAccount;
	}

	TokenAccount GetTokenAccount(const COutPoint& outpoint)
	{
		CTransaction tx;
		uint256 blockhash;
		if(!GetTransaction(outpoint.hash, tx, blockhash, true))
			throw std::runtime_error(strprintf("Input tx not found, txid=%s",outpoint.hash.ToString().c_str()));
		if(outpoint.n >= tx.vout.size())
			throw std::runtime_error(strprintf("tx.vin[0].prevout.n(%d) out of range, tx.vout.size()=%d",outpoint.n,tx.vout.size()));
		const CTxOut& vout = tx.vout[outpoint.n];
		if(vout.IsNull())
			throw std::runtime_error("Invalid prevout");
		CTxDestination txSenderDest;
		if(!ExtractDestination(vout.scriptPubKey,txSenderDest))
			throw std::runtime_error("Invalid tx sender");
		return CBitcoinAddress(txSenderDest);
	}
	
	TokenAccount GetTokenAccount(const CPubKey& pubkey)
	{
		return CBitcoinAddress(pubkey.GetID());
	}
	
	string CreateContractCallData(const string& function, const vector<string>& params)
	{
		string data;
		data += function;
		for(const auto& param : params)
		{
			if(param.length() > U256_STR_LEN)
				throw std::runtime_error("param too long");
			
			string pad(U256_STR_LEN-param.length(),'0');
			
			assert(pad.length()+param.length() == U256_STR_LEN);
			data += (pad + param);
		}
		return data;
	}

	string CreateContractCallData(const string& function, const string& param)
	{
		vector<string> vec(1,param);
		return CreateContractCallData(function,vec);
	}

	void ParseContractDataStack(bytes_t data, size_t n, std::stack<bytes_t>& dataStack)
	{
		if(data.empty())
			return;
		
		const byte_t* begin = data.data();
		const byte_t* end = data.data() + data.size();
		const byte_t* p = end;
		assert(begin && begin < end);
		for(int i=0; (i<n-1) && (p-begin > U256_BYTE_LEN); i++)
		{
			p -= U256_BYTE_LEN;
			assert(begin <= p && p < end);
			dataStack.push(bytes_t(p,p+U256_BYTE_LEN));
		}
		if(begin < p)
			dataStack.push(bytes_t(begin,p));
	}

	void ParseTokenCreateParams(const bytes_t& data, CAmount& supply)
	{
		std::stack<bytes_t> dataStack;
		const size_t stackSize = 9;
		ParseContractDataStack(data,stackSize,dataStack);
		
		// 0 608060...3d0029												  bytecode			   [stack top]
		// 1 0000000000000000000000000000000000000000000000000000000000000080 ?
		// 2 00000000000000000000000000000000000000000000000000000000000000c0 ?
		// 3 0000000000000000000000000000000000000000000000000000000005f5e100 Supply   100000000
		// 4 0000000000000000000000000000000000000000000000000000000000000004 Decimal  4
		// 5 0000000000000000000000000000000000000000000000000000000000000006
		// 6 4254313334350000000000000000000000000000000000000000000000000000 Name	   BT1345
		// 7 0000000000000000000000000000000000000000000000000000000000000002
		// 8 4254000000000000000000000000000000000000000000000000000000000000 Symbol   BT		   [stack bottom]

		if(dataStack.size() < stackSize)
			throw std::runtime_error("Too few construct parameters");
		
		dataStack.pop(); // bytecode
		dataStack.pop(); // magic number 0x80
		dataStack.pop(); // magic number 0xc0
	
		bytes_t supplyBytes = dataStack.top();
		std::reverse(supplyBytes.begin(),supplyBytes.end());
		uint256 supplyNum(supplyBytes);
		dataStack.pop(); // supply
	
		bytes_t decimalBytes = dataStack.top();
		std::reverse(decimalBytes.begin(),decimalBytes.end());
		uint256 decimalNum(decimalBytes);
		dataStack.pop(); // decimal

		size_t decimal = decimalNum.Get32();
		supply = supplyNum.Get64();
		
		// supply = pow(supply,decimal); ! pow() support double only
		for(int i=0; i<decimal; i++)
			supply *= 10;
		
		//dataStack.pop(); // length of name
		//dataStack.pop(); // name
		//dataStack.pop(); // length of symbol
		//dataStack.pop(); // symbol
	}
	
	void ParseTokenTransferParams(const bytes_t& data, TokenAccount& toAccount, CAmount& amount)
	{
		// 0 a9059cbb												          function		   [stack top]
		// 1 000000000000000000000000ef3b5a574042fdd935aa6099f5019445928d3c78 toAccount
		// 2 0000000000000000000000000000000000000000000000000000000000000001 amount		   [stack bottom]

		std::stack<bytes_t> dataStack;
		const size_t stackSize = 3;
		ParseContractDataStack(data,stackSize,dataStack);

		if(dataStack.size() < stackSize)
			throw std::runtime_error("Too few 'transfer' parameters");
		
		bytes_t funcBytes = dataStack.top();
		string function = HexStr(funcBytes);
		if(function != FUNC_CODE_transfer)
			throw std::runtime_error(strprintf("unknown function: %s",function.c_str()));
		dataStack.pop(); // function

		bytes_t accountBytes = dataStack.top();
		bytes_t::iterator start = accountBytes.begin();
		while(*start == byte_t(0))
			start++;
		string hexTokenAccount = HexStr(start,accountBytes.end());
		if(!IsHexTokenAccount(hexTokenAccount))
			throw std::runtime_error(strprintf("Invalid hex token account: %s",hexTokenAccount.c_str()));
		toAccount = GetTokenAccount(hexTokenAccount);
		if(!toAccount.IsGKC())
			throw std::runtime_error("Invalid token account");
		dataStack.pop(); // toAccount

		bytes_t amountBytes = dataStack.top();
		std::reverse(amountBytes.begin(),amountBytes.end());
		amount = uint256(amountBytes).Get64();
		dataStack.pop(); // amount
	}
	
	string CallContractAndGetOutput(const string& contractaddress, const string& data)
	{
		UniValue params(UniValue::VARR);
		params.push_back(contractaddress);
		params.push_back(data);
		UniValue result = callcontract(params,false);

		// Example:
		// ./gkc-cli callcontract 05bf1b2d49bf2eeb61c7b1d36a328fbd9c4809cf 70a08231000000000000000000000000783db0d65a2c145e8a79c8a588db14b03bdff59f
		// {
		//	 "address": "05bf1b2d49bf2eeb61c7b1d36a328fbd9c4809cf",
		//	 "executionResult": {
		//	"gasUsed": 23437,
		//	"excepted": "None",
		//	"newAddress": "05bf1b2d49bf2eeb61c7b1d36a328fbd9c4809cf",
		//	"output": "000000000000000000000000000000000000000000000000000000e8ce9c9160",   <--   we want this field
		//	"codeDeposit": 0,
		//	"gasRefunded": 0,
		//	"depositSize": 0,
		//	"gasForDeposit": 0
		//	 },
		//	 "transactionReceipt": {
		//	"stateRoot": "da111e677975c4acf710637fc04253aad14b9da5f8cc5b3d95b88a1d03a2dacb",
		//	"gasUsed": 23437,
		//	"bloom": "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
		//	"log": [
		//	]
		//	 }
		// }
		
		return result["executionResult"]["output"].get_str();
	}

	string SendtoContractAndGetTxid(const string& contractaddress, const string& data, const string& senderAddress, CAmount amount, CAmount gasLimit, double gasPrice)
	{
		UniValue params(UniValue::VARR);
		params.push_back(contractaddress);
		params.push_back(data);
		params.push_back(amount);
		params.push_back(gasLimit);
		params.push_back(UniValue(gasPrice));
		params.push_back(senderAddress);
		UniValue result = sendtocontract(params,false);

		// Example:
		// ./gkc-cli sendtocontract 9e27784a3d83ddc0c51240c4b257dba9b38f4241 a9059cbb000000000000000000000000ef3b5a574042fdd935aa6099f5019445928d3c780000000000000000000000000000000000000000000000000000000000000001 0 250000 0.00000041 GX5bpe3Z3QCGpQQAKaQpwpv7P4MmVFCD8F
		// {
		//   "txid": "02b9c37d6253142d93d6c4d9a92f1873a5266e53c0a6ffa2007448c85339d332",   <--   we want this field
		//   "sender": "GX5bpe3Z3QCGpQQAKaQpwpv7P4MmVFCD8F",
		//   "hash160": "912fefc8693b4bbcaa323e1bb4422da80ba76011"
		// }

		return result["txid"].get_str();
	}

	txnouttype GetScriptType(const CScript& script)
	{
		txnouttype type = TX_NONSTANDARD;
		vector<valtype> vSolutions;
		Solver(script, type, vSolutions); // function may be return false, but the 'type' will be matched.
		//printf("%s:%s:%d|script=%s,type=%d\n",__func__,__FILE__,__LINE__,script.ToString().c_str(),type);
		return type;
	}
	
	string listtokentransactions_entry_t::GetTypeString(type_t t)
	{
		switch(t)
		{
		case type_t::Create: return "create";
		case type_t::Transfer: return "transfer";
		}
		throw std::logic_error("type undefined");
		return "";
	}
		

	listtokentransactions_entry_t::block_t::block_t():height(0){}
	UniValue listtokentransactions_entry_t::block_t::ToUniValue() const
	{
		UniValue obj(UniValue::VOBJ);
		obj.push_back(Pair("height",height));
		obj.push_back(Pair("hash",hash.ToString()));
		obj.push_back(Pair("time",(int64_t)time));
		return obj;
	}
	
	listtokentransactions_entry_t::output_t::output_t():vout(0){}
	UniValue listtokentransactions_entry_t::output_t::ToUniValue() const
	{
		UniValue obj(UniValue::VOBJ);
		obj.push_back(Pair("txid",txid.ToString()));
		obj.push_back(Pair("vout",(int64_t)vout));
		return obj;
	}

	listtokentransactions_entry_t::listtokentransactions_entry_t():amount(0),type(type_t::Create){}
	UniValue listtokentransactions_entry_t::ToUniValue() const
	{
		UniValue obj(UniValue::VOBJ);
		obj.push_back(Pair("type",GetTypeString(type)));
		obj.push_back(Pair("block",block.ToUniValue()));
		obj.push_back(Pair("output",output.ToUniValue()));
		obj.push_back(Pair("tokenid",tokenid));
		obj.push_back(Pair("from",from));
		obj.push_back(Pair("to",to));
		obj.push_back(Pair("amount",amount));
		return obj;
	}

	listtokentransactions_entry_t listtokentransactions_entry_t::create(const CBlockIndex* pindex, const CTransaction& tx, size_t vout)
	{
		listtokentransactions_entry_t entry;
		entry.block.height = pindex->nHeight;
		entry.block.hash = pindex->GetBlockHash();
		entry.block.time = pindex->nTime;
		entry.output.txid = tx.GetHash();
		entry.output.vout = vout;

		if(vout >= tx.vout.size())
			throw std::runtime_error("vout out of range");
		
		const CScript& script = tx.vout[vout].scriptPubKey;
		
		std::vector<valtype> stack;
		if(!EvalScript(stack, script, SCRIPT_EXEC_BYTE_CODE, BaseSignatureChecker(),nullptr))
			throw std::runtime_error("Invalid contract script");

		txnouttype type = GetScriptType(script);
		switch(type)
		{
			case TX_CREATE:
			{
				entry.type = type_t::Create;
				entry.tokenid = CreateContractAddress(tx.GetHash(),vout);
				
				if(tx.vin.size() < 1)
					throw std::runtime_error("tx.vin too small");
				entry.to = GetTokenAccount(tx.vin[0].prevout).ToString();
			
				// mTemplates.insert(make_pair(TX_CREATE, CScript() << OP_VERSION << OP_GAS_LIMIT << OP_GAS_PRICE << OP_DATA << OP_CREATE));
				if(stack.size() < 5)
					throw std::runtime_error("Stack is too small");
			
				stack.pop_back(); // opcode
			
				const bytes_t& data = stack.back();
				ParseTokenCreateParams(data,entry.amount);
				stack.pop_back(); // data
			
				//stack.pop_back(); // nGasPrice
				//stack.pop_back(); // nGasLimit
				//stack.pop_back(); // VersionVM
				break;
			}
			case TX_CALL:
			{
				entry.type = type_t::Transfer;
			
				if(tx.vin.size() < 1)
					throw std::runtime_error("tx.vin too small");
				entry.from = GetTokenAccount(tx.vin[0].prevout).ToString();
			
				//mTemplates.insert(make_pair(TX_CALL, CScript() << OP_VERSION << OP_GAS_LIMIT << OP_GAS_PRICE << OP_DATA << OP_PUBKEYHASH << OP_CALL));
				if(stack.size() < 6)
					throw std::runtime_error("Stack is too small");
				
				stack.pop_back(); // opcode
			
				entry.tokenid = HexStr(stack.back());
				stack.pop_back(); // contractaddress(tokenid)
				
				const bytes_t& data = stack.back();
				TokenAccount toAccount;
				ParseTokenTransferParams(data,toAccount,entry.amount);
				entry.to = toAccount.ToString();
				stack.pop_back(); // datahex
				
				//stack.pop_back(); // nGasPrice
				//stack.pop_back(); // nGasLimit
				//stack.pop_back(); // VersionVM
				break;
			}
			case TX_CALL2:
			{
				entry.type = type_t::Transfer;
				//mTemplates.insert(make_pair(TX_CALL2, CScript() << OP_VERSION << OP_GAS_LIMIT << OP_GAS_PRICE << OP_DATA << OP_PUBKEYHASH << OP_SENDER_PUBKEY << OP_SENDER_SIG << OP_CALL));
				if(stack.size() < 8)
					throw std::runtime_error("Stack is too small");
			
				stack.pop_back(); // opcode
				stack.pop_back(); // senderSignature				
			
				entry.from = GetTokenAccount(CPubKey(stack.back())).ToString();
				stack.pop_back(); // senderPubkey
			
				entry.tokenid = HexStr(stack.back());
				stack.pop_back(); // contractaddress(tokenid)
			
				const bytes_t& data = stack.back();
				TokenAccount toAccount;
				ParseTokenTransferParams(data,toAccount,entry.amount);
				entry.to = toAccount.ToString();
				stack.pop_back(); // datahex
				
				//stack.pop_back(); // nGasPrice
				//stack.pop_back(); // nGasLimit
				//stack.pop_back(); // VersionVM
				break;
			}
			default:
				throw std::runtime_error(strprintf("Unhandled enum type: %d",(int)type));
		}
		
		return entry;
	}

	bool IsTokenTransfer(const CTxOut& txout)
	{
		return txout.scriptPubKey.HasOpCreate() || txout.scriptPubKey.HasOpCall();
	}
}

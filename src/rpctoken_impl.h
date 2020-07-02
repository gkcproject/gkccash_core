
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
#include "scriptex.h" // util::BytesToHexstd::string

namespace rpctoken_impl
{
	typedef unsigned char byte_t;
	typedef std::vector<byte_t> bytes_t;
	typedef uint160 Tokenid;
	typedef CBitcoinAddress TokenAccount;

	// symbol of ERC20 functions:
	extern const string FUNC_CODE_balanceOf;
	extern const string FUNC_CODE_transfer;

	bool IsTokenid(const std::string& str);
	bool IsTokenAccount(const std::string& str);
	bool IsHexTokenAccount(const std::string& str);
	bool IsTokenTransfer(const CTxOut& txout);

	void CheckTokenid(const std::string& tokenid);
	void CheckBlockHeight(BlockHeight h);
	void CheckTokenAccount(const std::string& account);
	void CheckAmount(const CAmount& amount);

	std::string GetHexTokenAccount(const std::string& tokenAccount);
	TokenAccount GetTokenAccount(const std::string& hexTokenAccount);
	TokenAccount GetTokenAccount(const COutPoint& outpoint);
	TokenAccount GetTokenAccount(const CPubKey& pubkey);
	
	txnouttype GetScriptType(const CScript& script);

	std::string CreateContractCallData(const std::string& function, const std::vector<std::string>& params);
	std::string CreateContractCallData(const std::string& function, const std::string& param);

	void ParseContractDataStack(bytes_t data, size_t n, std::stack<bytes_t>& dataStack);
	void ParseTokenCreateParams(const bytes_t& data, CAmount& supply);
	void ParseTokenTransferParams(const bytes_t& data, TokenAccount& toAccount, CAmount& amount);
	
	std::string CallContractAndGetOutput(const std::string& contractaddress, const std::string& data);
	std::string SendtoContractAndGetTxid(const std::string& contractaddress, const std::string& data, const std::string& senderAddress, CAmount amount =0, CAmount gasLimit =250000, double gasPrice =0.00000041);

	
	class listtokentransactions_entry_t
	{
	public:
		enum type_t : int
		{
			Create,
			Transfer,
		};

		static std::string GetTypeString(type_t t);
			
		class block_t
		{
		public:
			BlockHeight height;
			uint256     hash;
			time_t      time;
			
			block_t();
			UniValue ToUniValue() const;
		};
		
		class output_t
		{
		public:
			uint256 txid;
			size_t  vout;
			
			output_t();
			UniValue ToUniValue() const;
		};

		type_t   type;
		block_t  block;
		output_t output;
		std::string   tokenid;
		std::string   from;
		std::string   to;
		CAmount  amount;

		listtokentransactions_entry_t();
		UniValue ToUniValue() const;

		static listtokentransactions_entry_t create(const CBlockIndex* pindex, const CTransaction& tx, size_t vout);
	};

} // ~namespace

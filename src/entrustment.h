
#ifndef ENTRUSTMENT_H
#define ENTRUSTMENT_H

#ifdef DPOS

#include "primitives/transaction.h" // COutPoint
#include "amount.h" // CAmount
#include <map>
#include <iostream> // std::ostream
#include <sstream> // std::ostringstream
#include <string>
#include <vector>
#include "base58.h" // CBitcoinAddress
#include "chain.h" // BlockHeight
#include "script/script.h"
#include "univalue.h"
#include "sync.h" //CCriticalSection
#include "crp/CoinReleasePlan.h"
#include "db.h"
#include "serialize.h"

class CBlock;
class CTxOut;

namespace dpos
{
	typedef BlockHeight BlockAmount;
	typedef CAmount CoinAmount;
	
	class MonthlyCount {
	public:
		BlockAmount blockAmount;
		CoinAmount entrustAmount;
		uint256    reward_factor;
		CoinAmount expect_reward;
		MonthlyCount();
		void StartNewMonth();
	};

	class DposMetaBlock;

	class UserStatistics {
	public:
		CAmount entrustAmount;
		CAmount totalDealDivideReward;
		CAmount totalDividendReward;
		UserStatistics();
		UniValue ToUniValue() const;
	};
}
class Agent
{
public:
	AgentID id;
	std::string name;
	CAmount receivedTotalAmount;
	std::map<CBitcoinAddress,dpos::UserStatistics> userStatistics;
	crp::BlockAmount generatedBlockAmount;
	int64_t lastBlockTime;

	dpos::MonthlyCount last_season;
	dpos::MonthlyCount monthlyCount;

	CAmount last_season_reward;
	

	CAmount GetTotalDealDivideAmount() const;
	CAmount GetTotalDividendAmount() const;

	ADD_SERIALIZE_METHODS;

	template <typename Stream, typename Operation>
	inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
		if(ser_action.ForRead())
		{
			// to nothing
		}
		else
		{
			READWRITE(id);
			READWRITE(receivedTotalAmount);
			READWRITE((uint32_t)userStatistics.size());
			READWRITE(GetTotalDealDivideAmount());
			READWRITE(GetTotalDividendAmount());
			if(nVersion >= 2)
				READWRITE(name);
			if(nVersion >= 3)
				READWRITE(lastBlockTime);
		}
	}

	Agent();
	Agent(AgentID);
	AgentID Id()const;
	void SetName(const std::string&);
	std::string Name() const;
	void AddBlock(int64_t blocktime);
	CAmount Amount()const;
	void AddAmount(const CBitcoinAddress&, CAmount);
	void DelAmount(const CBitcoinAddress&, CAmount);
	void AddDealDivideReward(BlockHeight height, CBitcoinAddress,CAmount);
	void AddDividendReward(BlockHeight height, CBitcoinAddress,CAmount);
	UniValue ToUniValue() const;

	//bool SerializeFor_getagent(CDataStream&) const;
	size_t MemorySize() const;

	friend std::ostream& operator<<(std::ostream&, const Agent&);
	friend std::istream& operator>>(std::istream&, Agent&);
};

class SortRule
{
public:
	virtual ~SortRule(){}
	virtual bool Compare(const Agent* a, const Agent* b) const = 0;
};

//fucntional for sort Agent
class SortComparer
{
	const SortRule& rule;
public:
	SortComparer(const SortRule& r):rule(r){}
 	bool operator() (const Agent* a, const Agent* b) const {
		return rule.Compare(a,b);
 	}
};

class DescBySeasonEntrustAmount : public SortRule
{
public:
	bool Compare(const Agent* a, const Agent* b) const override;
};

class DescBySeasonRewardFactor : public SortRule
{
public:
	bool Compare(const Agent* a, const Agent* b) const override;
};

typedef std::map<AgentID,Agent> AgentMap;
typedef std::map<BlockHeight,AgentMap> AgentMapMap;

class Entrustment
{
public:
	Entrustment();

	BlockHeight forkHeightForLockMinerRewardReceiver;
	BlockHeight forkHeightForSpecifyMinerRewardReceiver;
	BlockHeight forkHeightForHalfAmountLimit;

	bool IsSeasonRewardV2(BlockHeight) const;

	// test if I am an agent node
	bool IsAgent() const;
	
	Agent* GetAgent(BlockHeight,AgentID);
	const Agent* GetAgent(BlockHeight,AgentID)const;
	Agent* GetMyAgent(BlockHeight);
	
	bool AddMetaBlock(BlockHeight, dpos::DposMetaBlock&);
	std::vector<Agent> GetAgentList(BlockHeight)const;

	void SetMyAgentID(AgentID);
	AgentID GetMyAgentID()const;

	std::map<CBitcoinAddress,CAmount> GetBonusMap(BlockHeight height, AgentID agentid, CAmount blockBonus) const;

	CAmount GetCreateAgentAmount(BlockHeight blockheight) const;
	CAmount GetMinEntrustAmount(BlockHeight blockheight) const;
	BlockHeight GetLockHeightForDeprive() const;
	
	bool CheckCoinStake(BlockHeight blockheight, const CTransaction& coinstake);
	bool CheckTransaction(const CTransaction&) const;

	CAmount GetRemainReward(BlockHeight baseHeight, const AgentID&);
	
	void RevertToHeight(BlockHeight);

	std::string ToString() const;
	UniValue ToUniValue() const;

	bool IsAgentIdValid(AgentID) const;

	bool InitAgent(BlockHeight,AgentID, const std::string& name);
	void DelAgent(BlockHeight,AgentID);
	std::map<CBitcoinAddress,CAmount> GetExpectDivideRewardMap(BlockHeight height, AgentID agentid) const;
	std::map<CBitcoinAddress,CAmount> GetDealDivideRewardMap(BlockHeight newBlockHeight, AgentID agentid) const;
	std::map<CBitcoinAddress,CAmount> GetMonthlyRewardMap(BlockHeight) const;
	std::vector<std::pair<const Agent*,CAmount>> GetSeasonReward(BlockHeight base_height, BlockHeight new_height) const;
	BlockHeight GetBaseHeight(BlockHeight newHeight) const;

	void WriteToBackupFile(BlockHeight);
	bool WriteAgentMapToFile(const AgentMap&, std::ofstream&);
	bool ReadFromBackupFile(BlockHeight);
	static std::string GetBackupFileName(BlockHeight);
	void StartNewMonth(BlockHeight);

	vector<Agent*> GetAgentsRank(BlockHeight height, int top_count, const SortRule&);

	void Lock();
	void Unlock();

protected:
	bool AddAgentTx(BlockHeight, CTransaction&);
	bool DelAgentTx(BlockHeight, CTransaction&);
	bool AddEntrust(BlockHeight, CBitcoinAddress, AgentID, CAmount);
	bool DepriveEntrust(BlockHeight, CBitcoinAddress, AgentID, CAmount);

	void InitHeightAmount(BlockHeight);
	BlockHeight LowestHeight() const;
	bool IsHeightExists(BlockHeight) const;
	dpos::UserStatistics* GetUserStatistics(BlockHeight, const AgentID&, const CBitcoinAddress&);
	const dpos::UserStatistics* GetUserStatistics(BlockHeight, const AgentID&, const CBitcoinAddress&) const;
	void CalcSeasonRewardFactor(BlockHeight height);
	void CalcExpectRewardOfSeason(BlockHeight);
	bool IsEntrustTxIsLocking(const uint256& txid, BlockHeight chainHeight) const;

private:
	AgentID myAgentId;

	CAmount createAgentAmount;
	CAmount minEntrustAmount;
	CAmount minDivideReward;
	BlockHeight depriveLockHeight;
	BlockHeight baseHeightOffset;

	AgentMapMap agentMaps;

	
	CCriticalSection cs_heights;

protected:
	AgentMap* GetAgentMapWithMaxHeight();
	const AgentMap* GetAgentMapWithMaxHeight() const;

public:
	static Entrustment& GetInstance();
};

std::ostream& operator<< (std::ostream&, const Entrustment&);

namespace dpos
{
	bool GetConfirmedTxOut(COutPoint output, CTxOut& txout);
	bool FindConfirmedTransaction(const uint256& txid, CTransaction&);
	bool CheckCoinStake(const CBlock&);
	bool HaveDposArgv(const std::map<std::string,std::string>& argsMap);
	void AppendEntrustReward(const std::map<CBitcoinAddress,CAmount>& entrustDivideMap, std::vector<CTxOut>& vout);
	void AppendMonthlyReward(const std::map<CBitcoinAddress,CAmount>& rewardMap, std::vector<CTxOut>& vout);
	CBitcoinAddress GetReceiveAddressOfAgent(const AgentID&);
	Entrustment& GetInstance();
	bool AddMetaBlock(CBlockIndex*);
	CScript GetCreatorPubKeyOf(AgentID);
	vector<CBitcoinAddress> GetAddressesInScript(const CScript&);


	class DposMetaBlock
	{
	public:
		class Create {
		public:
			AgentID agentid;
			string  name;

			ADD_SERIALIZE_METHODS;
			template <typename Stream, typename Operation>
			inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
				READWRITE(agentid);
				READWRITE(name);
			}
		};
		class Resign {
		public:
			AgentID agentid;
			
			ADD_SERIALIZE_METHODS;
			template <typename Stream, typename Operation>
			inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
				READWRITE(agentid);
			}
		};
		
		enum EntrustType { ENTRUST, DEPRIVE };
		class Entrust {
		public:
			string             addr;
			/*EntrustType*/int type;
 			AgentID 		   agentid;
 			CAmount 		   amount;
 			uint256 		   txid;
 			int 			   vout_i;
	
			ADD_SERIALIZE_METHODS;
			template <typename Stream, typename Operation>
			inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
				READWRITE(addr);
				READWRITE(type);
				READWRITE(agentid);
				READWRITE(amount);
				READWRITE(txid);
				READWRITE(vout_i);
			}
		};
			
		enum class RewardType { ENTRUST, SEASON };
		class Reward {
		public:
			string            addr;
			/*RewardType*/int type;
			CAmount 		  amount;
			
			ADD_SERIALIZE_METHODS;
			template <typename Stream, typename Operation>
			inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
				READWRITE(addr);
				READWRITE(type);
				READWRITE(amount);
			}
		};
	
		DposMetaBlock();
		DposMetaBlock(const CBlock&);
		
		uint256 		blockhash;
		AgentID         agentid;
		int64_t         time;
		vector<Create>  create_list;
		vector<Resign>  resign_list;
		vector<Entrust> entrust_list;
		vector<Reward>	reward_list;
	
		ADD_SERIALIZE_METHODS;
		template <typename Stream, typename Operation>
		inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
			READWRITE(blockhash);
			READWRITE(agentid);
			READWRITE(time);
			READWRITE(create_list);
			READWRITE(resign_list);
			READWRITE(entrust_list);
			READWRITE(reward_list);
		}

		UniValue ToUniValue() const;
		string ToString() const;
	
	};

	class DposMetaBlockIO {
	public:
		CDiskBlockPos last_block_pos;
		CCriticalSection cs_last_file;

		static DposMetaBlockIO& GetInstance();

		bool LoadLastBlockPos();
		bool Write(const DposMetaBlock&);
		bool Read(CDiskBlockPos, DposMetaBlock&);

		/** Open a dpos meta block file (dps?????.dat) */
		FILE* OpenDposMetaBlockFile(const CDiskBlockPos& pos, bool fReadOnly = false);

	protected:
		DposMetaBlockIO(){
		}

	};
	
	bool LoadEntrustmentCache();
	
	class DposMetaBlockIndexDB : public CDB
	{
	public:
		static DposMetaBlockIndexDB& GetInstance();
		
		bool WriteMetaBlockIndex(uint256 blockhash, const CDiskBlockPos&);
		bool ReadMetaBlockIndex(uint256 blockhash, CDiskBlockPos&);
		bool ExistsBlock(uint256 blockhash);
		bool WriteLastPos(const CDiskBlockPos& pos);
		bool ReadLastPos(CDiskBlockPos&);

	protected:
		DposMetaBlockIndexDB(const std::string& strFilename, const char* pszMode = "r+") : CDB(strFilename, pszMode) {}
	
		CCriticalSection cs_db;
	};

	bool WriteDposMetaBlock(const CBlock&);
}

#endif // ~DPOS

#endif // ~ENTRUSTMENT_H

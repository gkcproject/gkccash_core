#ifndef CONTRACT_COMPONENT_H
#define CONTRACT_COMPONENT_H

#pragma once

#include <stdint.h>
#include "util.h"

#include "chain.h"
#include "txmempool.h"

/////////////////////////////////////////// //gkc-vm
#include "gkcstate.h"
#include "gkcDGP.h"
#include "gkctransaction.h"
#include <libethereum/ChainParams.h>
#include <libethashseal/Ethash.h>
#include <libethashseal/GenesisInfo.h>
#include <script/standard.h>
#include <serialize.h>
#include <libethereum/Transaction.h>
#include <libethereum/LastBlockHashesFace.h>

#include "contractbase.h"
#include "storageresults.h"
#include "univalue.h"

using valtype = std::vector<unsigned char>;
///////////////////////////////////////////

struct EthTransactionParams
{
    VersionVM version;
    dev::u256 gasLimit;
    dev::u256 gasPrice;
    valtype code;
    dev::Address receiveAddress;

    bool operator!=(EthTransactionParams etp)
    {
        if (this->version.toRaw() != etp.version.toRaw() || this->gasLimit != etp.gasLimit ||
            this->gasPrice != etp.gasPrice || this->code != etp.code ||
            this->receiveAddress != etp.receiveAddress)
            return true;
        return false;
    }
};

using ExtractGkcTX = std::pair<std::vector<GkcTransaction>, std::vector<EthTransactionParams>>;


class GkcTxConverter
{

public:

    GkcTxConverter(CTransaction tx, CCoinsViewCache *v = NULL, const std::vector<CTransaction> *blockTxs = NULL)
            : txBit(tx), view(v), blockTransactions(blockTxs)
    {
    }

    bool extractionGkcTransactions(ExtractGkcTX &gkcTx);

private:

    bool receiveStack(const CScript &scriptPubKey);

    bool parseEthTXParams(EthTransactionParams &params);

    GkcTransaction createEthTX(const EthTransactionParams &etp, const uint32_t nOut);

    const CTransaction txBit;
    const CCoinsViewCache *view;
    std::vector<valtype> stack;
    opcodetype opcode;
    const std::vector<CTransaction> *blockTransactions;

};

class LastHashes: public dev::eth::LastBlockHashesFace
{
public:
    explicit LastHashes();

    void set(CBlockIndex const* tip);

    dev::h256s precedingHashes(dev::h256 const&) const;

    void clear();

private:
    dev::h256s m_lastHashes;
};


class ByteCodeExec
{

public:

    ByteCodeExec(const CBlock &_block, std::vector<GkcTransaction> _txs, const uint64_t _blockGasLimit) : txs(_txs),
                                                                                                           block(_block),
                                                                                                           blockGasLimit(
                                                                                                                   _blockGasLimit)
    {
    }

    bool performByteCode(dev::eth::Permanence type = dev::eth::Permanence::Committed,int nHeight = 0);

    bool processingResults(ByteCodeExecResult &result);

    std::vector<ResultExecute> &getResult()
    {
        return result;
    }

private:


    dev::eth::EnvInfo BuildEVMEnvironment(int nHeight);

    dev::Address EthAddrFromScript(const CScript &scriptIn);

    std::vector<GkcTransaction> txs;

    std::vector<ResultExecute> result;

    const CBlock &block;
    const uint64_t blockGasLimit;

    LastHashes lastHashes;
};

bool ComponentInitialize();

bool ContractInit();

bool ComponentStartup();

bool ComponentShutdown();

uint64_t GetMinGasPrice(int height);

uint64_t GetBlockGasLimit(int height);
uint32_t GetBlockSize(int height);

bool AddressInUse(string contractaddress);

bool CheckContractTx(const CTransaction tx, CAmount &nFees,
                         CAmount &nMinGasPrice, int &level,
                         string &errinfo, const CAmount nAbsurdFee = 0, bool rawTx = false);

bool RunContractTx(CTransaction tx, CCoinsViewCache *v, CBlock *pblock,
                       uint64_t minGasPrice,
                       uint64_t hardBlockGasLimit,
                       uint64_t softBlockGasLimit,
                       uint64_t txGasLimit,
                       uint64_t usedGas,
                       ByteCodeExecResult &testExecResult);

bool ContractTxConnectBlock(CTransaction tx, uint32_t transactionIndex, CCoinsViewCache *v, const CBlock &block,
                                int nHeight,
                                ByteCodeExecResult &bcer,
                                bool bLogEvents,
                                bool fJustCheck,
                                std::map<dev::Address, std::pair<CHeightTxIndexKey, std::vector<uint256>>> &heightIndexes,
                                int &level, string &errinfo,uint64_t &countCumulativeGasUsed,uint64_t &blockGasUsed);

void GetState(uint256 &hashStateRoot, uint256 &hashUTXORoot);

void UpdateState(uint256 hashStateRoot, uint256 hashUTXORoot);

void DeleteResults(std::vector<CTransaction> const &txs);

std::vector<TransactionReceiptInfo> GetResult(uint256 const &hashTx);

void CommitResults();

void ClearCacheResult();

std::map<dev::h256, std::pair<dev::u256, dev::u256>> GetStorageByAddress(string address);

void SetTemporaryState(uint256 hashStateRoot, uint256 hashUTXORoot);

std::unordered_map<dev::h160, dev::u256> GetContractList();

CAmount GetContractBalance(dev::h160 address);

std::vector<uint8_t> GetContractCode(dev::Address address);

bool GetContractVin(dev::Address address, dev::h256 &hash, uint32_t &nVout, dev::u256 &value, uint8_t &alive);

void RPCCallContract(UniValue &result, const string addrContract, std::vector<unsigned char> opcode, string sender = "", uint64_t gasLimit = 0);

string GetExceptedInfo(uint32_t index);


#endif //CONTRACT_COMPONENT_H

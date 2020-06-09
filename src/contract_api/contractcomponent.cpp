#include "util.h"
#include "contractcomponent.h"
#include "contractbase.h"
#include "univalue/include/univalue.h"
#include "timedata.h"
#include "contractconfig.h"
#include "main.h"
#include "libdevcore/Common.h"
#include "libdevcore/Log.h"
//#include <libethereum/LastBlockHashesFace.h>
#include <fstream>
#include <boost/filesystem.hpp>

static std::unique_ptr<GkcState> globalState;
static std::shared_ptr<dev::eth::SealEngineFace> globalSealEngine;
static StorageResults *pstorageresult = NULL;
static bool fRecordLogOpcodes = false;
static bool fIsVMlogFile = false;
static bool fGettingValuesDGP = false;

/** Too high fee. Can not be triggered by P2P transactions */
//static const unsigned int REJECT_HIGHFEE = 0x100;

bool CheckMinGasPrice(std::vector<EthTransactionParams> &etps, const uint64_t &minGasPrice)
{
    for (EthTransactionParams &etp : etps)
    {
        if (etp.gasPrice < dev::u256(minGasPrice))
            return false;
    }
    return true;
}

valtype
GetSenderAddress(const CTransaction &tx, const CCoinsViewCache *coinsView, const std::vector<CTransaction> *blockTxs)
{
	bool newVersion = false;
	valtype senderSignature;
	CPubKey senderPubKey;
	{
		std::vector<valtype> stack;
		for (size_t i = 0; i < tx.vout.size(); i++)
		{
			const CScript& script = tx.vout[i].scriptPubKey;
			if (script.HasOpCreate() || script.HasOpCall())
			{
				if(!EvalScript(stack, script, SCRIPT_EXEC_BYTE_CODE, BaseSignatureChecker(),nullptr))
					continue;
				break;
			}
		}
		LogPrint("contract","GetSenderAddress | stack.size()=%d\n",stack.size());
		if(stack.size() > 0)
		{
			opcodetype opcode = (opcodetype)(*stack.back().begin());
			stack.pop_back(); // opcode
			LogPrint("contract","GetSenderAddress | opcode=%d\n",opcode);
			if(opcode==OP_CALL)
			{
				LogPrint("contract","GetSenderAddress | extend stack.size()=%d\n",stack.size());
				if(stack.size() >= 7)
				{
					newVersion = true;
					senderSignature = stack.back();
					stack.pop_back(); // senderSignature				
					senderPubKey = CPubKey(stack.back().begin(),stack.back().end());
					stack.pop_back(); // senderPubKey
				}
				stack.pop_back(); // contractaddress
				stack.pop_back(); // datahex
				stack.pop_back(); // nGasPrice
				stack.pop_back(); // nGasLimit
				stack.pop_back(); // VersionVM
			}
		}
	}

	if(newVersion)
	{
		CKeyID keyid = senderPubKey.GetID();
		return valtype(keyid.begin(),keyid.end());
	}

    CScript script;
    bool scriptFilled = false; //can't use script.empty() because an empty script is technically valid

    // First check the current (or in-progress) block for zero-confirmation change spending that won't yet be in txindex
    if (blockTxs)
    {
        LogPrintf("tx vin size %d\n", tx.vin.size());
        if (tx.vin.size() > 0)
            LogPrintf("tx vin[0] hash %s, n %d", tx.vin[0].prevout.hash.GetHex().c_str(),
                    tx.vin[0].prevout.n);
        
        LogPrintf("vtx size is %d\n\n", blockTxs->size());
        for (auto btx : *blockTxs)
        {
            LogPrintf("vtx vin size: %d, vout size %d\n", btx.vin.size(), btx.vout.size());
            LogPrintf("vtx vin[0] hash %s\n", btx.GetHash().GetHex().c_str());
            if (btx.GetHash() == tx.vin[0].prevout.hash)
            {LogPrintf("Find match");
                script = btx.vout[tx.vin[0].prevout.n].scriptPubKey;
                scriptFilled = true;
                break;
            }LogPrintf("Not match");
        }
    }LogPrintf("GetSenderAddress, scriptFilled: %d, coinsView %d\n", scriptFilled, NULL == coinsView);
    if (!scriptFilled && coinsView)
    {
        // script = coinsView->AccessCoin(tx.vin[0].prevout.hash).out.scriptPubKey;
        script = coinsView->AccessCoins(tx.vin[0].prevout.hash)->vout[tx.vin[0].prevout.n].scriptPubKey;
        scriptFilled = true;
    }LogPrintf("GetSenderAddress, script: %s\n", script.ToString().c_str());
    if (!scriptFilled)
    {

        CTransaction txPrevout;
        uint256 hashBlock;
        LogPrintf("before GetTransaction\n");
        if (GetTransaction(tx.vin[0].prevout.hash, txPrevout, hashBlock, true))
        {
            script = txPrevout.vout[tx.vin[0].prevout.n].scriptPubKey;
        }
        else
        {
            LogPrint("Error ","fetching transaction details of tx %s. This will probably cause more errors",
                     tx.vin[0].prevout.hash.ToString().c_str());
            return valtype();
        }
    }LogPrintf("before ExtractDestination\n");

    CTxDestination addressBit;
    txnouttype txType = TX_NONSTANDARD;
    if (ExtractDestination(script, addressBit, &txType))
    {
        LogPrintf(" ExtractDestination done\n");
        if ((txType == TX_PUBKEY || txType == TX_PUBKEYHASH) &&
                addressBit.type() == typeid(CKeyID))
        {
            CKeyID senderAddress(boost::get<CKeyID>(addressBit));
            return valtype(senderAddress.begin(), senderAddress.end());
        }
    }LogPrintf(" return type 0\n");
    //prevout is not a standard transaction format, so just return 0
    return valtype();
}


UniValue vmLogToJSON(const ResultExecute &execRes, const CTransaction &tx, const CBlock &block)
{
    UniValue result(UniValue::VOBJ);


    int height = chainActive.Tip()->nHeight;

    if (tx != CTransaction())
        result.push_back(Pair("txid", tx.GetHash().GetHex()));
    result.push_back(Pair("address", execRes.execRes.newAddress.hex()));
    if (block.GetHash() != CBlock().GetHash())
    {
        result.push_back(Pair("time", block.GetBlockTime()));
        result.push_back(Pair("blockhash", block.GetHash().GetHex()));
        result.push_back(Pair("blockheight", height + 1));

    } else
    {
        result.push_back(Pair("time", GetAdjustedTime()));
        result.push_back(Pair("blockheight", height));
    }
    UniValue logEntries(UniValue::VARR);
    dev::eth::LogEntries logs = execRes.txRec.log();
    for (dev::eth::LogEntry log : logs)
    {
        UniValue logEntrie(UniValue::VOBJ);
        logEntrie.push_back(Pair("address", log.address.hex()));
        UniValue topics(UniValue::VARR);
        for (dev::h256 l : log.topics)
        {
            UniValue topicPair(UniValue::VOBJ);
            topicPair.push_back(Pair("raw", l.hex()));
            topics.push_back(topicPair);
            //TODO add "pretty" field for human readable data
        }
        UniValue dataPair(UniValue::VOBJ);
        dataPair.push_back(Pair("raw", HexStr(log.data)));
        logEntrie.push_back(Pair("data", dataPair));
        logEntrie.push_back(Pair("topics", topics));
        logEntries.push_back(logEntrie);
    }
    result.push_back(Pair("entries", logEntries));
    return result;
}

void writeVMlog(const std::vector<ResultExecute> &res, const CTransaction &tx = CTransaction(),
                const CBlock &block = CBlock())
{
    boost::filesystem::path gkcDir = GetDataDir() / "vmExecLogs.json";
    std::stringstream ss;
    if (fIsVMlogFile)
    {
        ss << ",";
    } else
    {
        std::ofstream file(gkcDir.string(), std::ios::out | std::ios::app);
        file << "{\"logs\":[]}";
        file.close();
    }

    for (size_t i = 0; i < res.size(); i++)
    {
        ss << vmLogToJSON(res[i], tx, block).write();
        if (i != res.size() - 1)
        {
            ss << ",";
        } else
        {
            ss << "]}";
        }
    }

    std::ofstream file(gkcDir.string(), std::ios::in | std::ios::out);
    file.seekp(-2, std::ios::end);
    file << ss.str();
    file.close();
    fIsVMlogFile = true;
}



std::vector<ResultExecute> CallContract(const dev::Address &addrContract, std::vector<unsigned char> opcode,
                                        const dev::Address &sender = dev::Address(), uint64_t gasLimit = 0)
{
    CBlock block;
    CMutableTransaction tx;

    CBlockIndex *pTip = chainActive.Tip();

    CBlockIndex *pblockindex = mapBlockIndex[pTip->GetBlockHash()];

    ReadBlockFromDisk(block, pblockindex);
    block.nTime = GetAdjustedTime();
    block.vtx.erase(block.vtx.begin() + 1, block.vtx.end());

    GkcDGP gkcDGP(globalState.get(), fGettingValuesDGP);
    uint64_t blockGasLimit = gkcDGP.getBlockGasLimit(pTip->nHeight + 1);

    if (gasLimit == 0)
    {
        gasLimit = blockGasLimit - 1;
    }
    dev::Address senderAddress =
            sender == dev::Address() ? dev::Address("ffffffffffffffffffffffffffffffffffffffff") : sender;
    tx.vout.push_back(
                CTxOut(0, CScript() << OP_DUP << OP_HASH160 << senderAddress.asBytes() << OP_EQUALVERIFY << OP_CHECKSIG));
    block.vtx.push_back(CTransaction(tx));

    GkcTransaction callTransaction(0, 1, dev::u256(gasLimit), addrContract, opcode, dev::u256(0));
    callTransaction.forceSender(senderAddress);
    callTransaction.setVersion(VersionVM::GetEVMDefault());


    ByteCodeExec exec(block, std::vector<GkcTransaction>(1, callTransaction), blockGasLimit);
    exec.performByteCode(dev::eth::Permanence::Reverted);
    return exec.getResult();
}



bool ComponentInitialize()
{
    LogPrintStr("initialize CContract component");
    return true;
}

bool ContractInit()
{
    //FixME: Comment this is not right?

    ////////////////////////////////////////////////////////////////////// // gkc
//    dev::g_logPost = [&](std::string const& s, char const* c){ LogPrintStr(s + '\n', true); };
//    dev::g_logPost(std::string("\n\n\n\n\n\n\n\n\n\n"), NULL);
    //LogPrintStr("\n\n\n\n\n\n\nnnnnnnnnnnnnnnnnnnnnn",true);
    //////////////////////////////////////////////////////////////////////

    if ((GetBoolArg("-dgpstorage", false) && GetBoolArg("-dgpevm", false)) ||
            (!GetBoolArg("-dgpstorage", false) && GetBoolArg("-dgpevm", false)) ||
            (!GetBoolArg("-dgpstorage", false) && !GetBoolArg("-dgpevm", false)))
    {
        fGettingValuesDGP = true;
    } else
    {
        fGettingValuesDGP = false;
    }


    LogPrintf("AppInitMain: fGettingValuesDGP = %d\n", fGettingValuesDGP);

    dev::eth::Ethash::init();
    boost::filesystem::path stateDir = GetDataDir() / CONTRACT_STATE_DIR;
    bool fStatus = boost::filesystem::exists(stateDir);
    const std::string dirGkc(stateDir.string());
    const dev::h256 hashDB(dev::sha3(dev::rlp("")));
    dev::eth::BaseState existstate = fStatus ? dev::eth::BaseState::PreExisting : dev::eth::BaseState::Empty;
    globalState = std::unique_ptr<GkcState>(
                new GkcState(dev::u256(0), GkcState::openDB(dirGkc, hashDB, dev::WithExisting::Trust), dirGkc,
                              existstate));

    //Note: Should only add after genesis contract changed state and utxo root
    //dev::eth::ChainParams cp((dev::eth::genesisInfo(dev::eth::Network::gkcMainNetwork)));
    //globalSealEngine = std::unique_ptr<dev::eth::SealEngineFace>(cp.createSealEngine());

    pstorageresult = new StorageResults(stateDir.string());


    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (IsEnabled)
    {
        CBlockIndex *pTip = chainActive.Tip();
        CBlock block;
        if (!ReadBlockFromDisk(block, pTip))
        {
            LogPrint("ReadBlockFromDisk ","failed at %d, hash=%s", pTip->nHeight,
                     pTip->GetBlockHash().ToString());
            assert(0);
            return false;
        } else
        {
            uint256 hashStateRoot;
            uint256 hashUTXORoot;
            if(block.GetVMState(hashStateRoot, hashUTXORoot) != RET_VM_STATE_OK)
            {
                assert(0);
                LogPrintStr("GetVMState failed");
                return false;
            }else {
                globalState->setRoot(uintToh256(hashStateRoot));
                globalState->setRootUTXO(uintToh256(hashUTXORoot));
            }
        }
    } else
    {

        globalState->setRoot(dev::sha3(dev::rlp("")));
        globalState->setRootUTXO(dev::sha3(dev::rlp("")));
        //globalState->populateFrom(cp.genesisState);
        LogPrintf("globalState:%s globalUtxo:%s\n",h256Touint(dev::sha3(dev::rlp(""))).GetHex().c_str(),h256Touint(dev::sha3(dev::rlp(""))).GetHex().c_str());

    }

    globalState->db().commit();
    globalState->dbUtxo().commit();

    fRecordLogOpcodes = GetBoolArg("-record-log-opcodes", false);
    fIsVMlogFile = boost::filesystem::exists(GetDataDir() / "vmExecLogs.json");

    if (!fLogEvents)
    {
        pstorageresult->wipeResults();
    }

    return true;
}


bool ComponentStartup()
{
    LogPrintStr("starting CContract component\n");

    return true;
}

bool ComponentShutdown()
{
    LogPrintStr("shutdown CContract component");

    delete pstorageresult;
    pstorageresult = NULL;
    delete globalState.release();
    //globalSealEngine.reset();
    return true;
}

uint64_t GetMinGasPrice(int height)
{
    uint64_t minGasPrice = 1;

    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (!IsEnabled)
    {
        return 0;
    }

    GkcDGP gkcDGP(globalState.get(), fGettingValuesDGP);
    //globalSealEngine->setGkcSchedule(gkcDGP.getGasSchedule(height));
    minGasPrice = gkcDGP.getMinGasPrice(height);

    return minGasPrice;
}

uint64_t GetBlockGasLimit(int height)
{
    uint64_t blockGasLimit = 1;

    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (!IsEnabled)
    {
        return 0;
    }

    GkcDGP gkcDGP(globalState.get(), fGettingValuesDGP);
    //globalSealEngine->setGkcSchedule(gkcDGP.getGasSchedule(height));
    blockGasLimit = gkcDGP.getBlockGasLimit(height);

    return blockGasLimit;
}

uint32_t GetBlockSize(int height)
{
    uint32_t blockSize = 1;

    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (!IsEnabled)
    {
        return 0;
    }

    GkcDGP gkcDGP(globalState.get(), fGettingValuesDGP);
    //globalSealEngine->setGkcSchedule(gkcDGP.getGasSchedule(height));
    blockSize = gkcDGP.getBlockSize(height);

    return blockSize;
}

bool AddressInUse(string contractaddress)
{
    //
    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (!IsEnabled)
    {
        return false;
    }
    dev::Address addrAccount(contractaddress);
    return globalState->addressInUse(addrAccount);
}

bool CheckContractTx(const CTransaction tx,CAmount &nFees,
                     CAmount &nMinGasPrice, int &level,
                     string &errinfo, const CAmount nAbsurdFee, bool rawTx)
{
    dev::u256 txMinGasPrice = 0;


    int height = chainActive.Tip()->nHeight + 1;

    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (!IsEnabled)
    {
        return false;
    }

    uint64_t minGasPrice = GetMinGasPrice(height);
    uint64_t blockGasLimit = GetBlockGasLimit(height);
    size_t count = 0;
    for (const CTxOut &o : tx.vout)
        count += o.scriptPubKey.HasOpCreate() || o.scriptPubKey.HasOpCall() ? 1 : 0;
    GkcTxConverter converter(tx, NULL);
    ExtractGkcTX resultConverter;
    LogPrintStr("CheckContractTx"); //gkc debug
    if (!converter.extractionGkcTransactions(resultConverter))
    {
        level = 100;
        errinfo = "bad-tx-bad-contract-format";
        return false;
    }
    std::vector<GkcTransaction> gkcTransactions = resultConverter.first;
    std::vector<EthTransactionParams> gkcETP = resultConverter.second;

    dev::u256 sumGas = dev::u256(0);
    dev::u256 gasAllTxs = dev::u256(0);
    for (GkcTransaction gkcTransaction : gkcTransactions)
    {
        sumGas += gkcTransaction.gas() * gkcTransaction.gasPrice();

        if (sumGas > dev::u256(INT64_MAX))
        {
            level = 100;
            errinfo = "bad-tx-gas-stipend-overflow";
            return false;
        }

        if (sumGas > dev::u256(nFees))
        {
            level = 100;
            errinfo = "bad-txns-fee-notenough";
            return false;
        }

        if (txMinGasPrice != 0)
        {
            txMinGasPrice = std::min(txMinGasPrice, gkcTransaction.gasPrice());
        } else
        {
            txMinGasPrice = gkcTransaction.gasPrice();
        }
        VersionVM v = gkcTransaction.getVersion();
        if (v.format != 0)
        {
            level = 100;
            errinfo = "bad-tx-version-format";
            return false;
        }
        if (v.rootVM != 1)
        {
            level = 100;
            errinfo = "bad-tx-version-rootvm";
            return false;
        }
        if (v.vmVersion != 0)
        {
            level = 100;
            errinfo = "bad-tx-version-vmversion";
            return false;
        }
        if (v.flagOptions != 0)
        {
            level = 100;
            errinfo = "bad-tx-version-flags";
            return false;
        }

        //check gas limit is not less than minimum mempool gas limit
        if (gkcTransaction.gas() < GetBoolArg("-minmempoolgaslimit", MEMPOOL_MIN_GAS_LIMIT))
        {
            level = 100;
            errinfo = "bad-tx-too-little-mempool-gas";
            return false;
        }

        //check gas limit is not less than minimum gas limit (unless it is a no-exec tx)
        if (gkcTransaction.gas() < MINIMUM_GAS_LIMIT && v.rootVM != 0)
        {
            level = 100;
            errinfo = "bad-tx-too-little-gas";
            return false;
        }

        if (gkcTransaction.gas() > UINT32_MAX)
        {
            level = 100;
            errinfo = "bad-tx-too-much-gas";
            return false;
        }

        gasAllTxs += gkcTransaction.gas();
        if (gasAllTxs > dev::u256(blockGasLimit))
        {
            level = 1;
            errinfo = "bad-txns-gas-exceeds-blockgaslimit";
            return false;
        }

        //don't allow less than DGP set minimum gas price to prevent MPoS greedy mining/spammers
        if (v.rootVM != 0 && (uint64_t)gkcTransaction.gasPrice() < minGasPrice)
        {
            level = 100;
            errinfo = "bad-tx-low-gas-price";
            return false;
        }
    }

    if (!CheckMinGasPrice(gkcETP, minGasPrice))
    {
        level = 100;
        errinfo = "bad-txns-small-gasprice";
        return false;
    }

    if (count > gkcTransactions.size())
    {
        level = 100;
        errinfo = "bad-txns-incorrect-format";
        return false;
    }

    if (rawTx && nAbsurdFee && dev::u256(nFees) > dev::u256(nAbsurdFee) + sumGas)
    {
        level = REJECT_HIGHFEE;
        errinfo = "absurdly-high-fee" + strprintf("%d > %d", nFees, nAbsurdFee);
        return false;
    }

    nMinGasPrice = CAmount(txMinGasPrice);

    nFees -= CAmount(sumGas);

    return true;
}

bool RunContractTx(CTransaction tx, CCoinsViewCache *v, CBlock *pblock,
                   uint64_t minGasPrice,
                   uint64_t hardBlockGasLimit,
                   uint64_t softBlockGasLimit,
                   uint64_t txGasLimit,
                   uint64_t usedGas,
                   ByteCodeExecResult &testExecResult)
{
    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (!IsEnabled)
    {
        return false;
    }

    GkcTxConverter convert(tx, v, &pblock->vtx);

    ExtractGkcTX resultConverter;
    LogPrintStr("RunContractTx"); //gkc debug
    if (!convert.extractionGkcTransactions(resultConverter))
    {
        //this check already happens when accepting txs into mempool
        //therefore, this can only be triggered by using raw transactions on the staker itself
        return false;
    }
    std::vector<GkcTransaction> gkcTransactions = resultConverter.first;
    dev::u256 txGas = 0;
    for (GkcTransaction gkcTransaction : gkcTransactions)
    {
        txGas += gkcTransaction.gas();
        if (txGas > txGasLimit)
        {
            // Limit the tx gas limit by the soft limit if such a limit has been specified.
            return false;
        }

        if (usedGas + gkcTransaction.gas() > softBlockGasLimit)
        {
            //if this transaction's gasLimit could cause block gas limit to be exceeded, then don't add it
            return false;
        }
        if (gkcTransaction.gasPrice() < minGasPrice)
        {
            //if this transaction's gasPrice is less than the current DGP minGasPrice don't add it
            return false;
        }
    }
    // We need to pass the DGP's block gas limit (not the soft limit) since it is consensus critical.
    ByteCodeExec exec(*pblock, gkcTransactions, hardBlockGasLimit);
    if (!exec.performByteCode())
    {
        //error, don't add contract
        return false;
    }

    if (!exec.processingResults(testExecResult))
    {
        return false;
    }
    return true;
}

const std::map<std::uint32_t, std::string> exceptionMap =
{
    {0,  "None"},
    {1,  "Unknown"},
    {2,  "BadRLP"},
    {3,  "InvalidFormat"},
    {4,  "OutOfGasIntrinsic"},
    {5,  "InvalidSignature"},
    {6,  "InvalidNonce"},
    {7,  "NotEnoughCash"},
    {8,  "OutOfGasBase"},
    {9,  "BlockGasLimitReached"},
    {10, "BadInstruction"},
    {11, "BadJumpDestination"},
    {12, "OutOfGas"},
    {13, "OutOfStack"},
    {14, "StackUnderflow"},
    {15, "CreateWithValue"},
};

uint32_t GetExcepted(dev::eth::TransactionException status)
{
    uint32_t index = 0;
    if (status == dev::eth::TransactionException::None)
    {
        index = 0;
    } else if (status == dev::eth::TransactionException::Unknown)
    {
        index = 1;
    } else if (status == dev::eth::TransactionException::BadRLP)
    {
        index = 2;
    } else if (status == dev::eth::TransactionException::InvalidFormat)
    {
        index = 3;
    } else if (status == dev::eth::TransactionException::OutOfGasIntrinsic)
    {
        index = 4;
    } else if (status == dev::eth::TransactionException::InvalidSignature)
    {
        index = 5;
    } else if (status == dev::eth::TransactionException::InvalidNonce)
    {
        index = 6;
    } else if (status == dev::eth::TransactionException::NotEnoughCash)
    {
        index = 7;
    } else if (status == dev::eth::TransactionException::OutOfGasBase)
    {
        index = 8;
    } else if (status == dev::eth::TransactionException::BlockGasLimitReached)
    {
        index = 9;
    } else if (status == dev::eth::TransactionException::BadInstruction)
    {
        index = 10;
    } else if (status == dev::eth::TransactionException::BadJumpDestination)
    {
        index = 11;
    } else if (status == dev::eth::TransactionException::OutOfGas)
    {
        index = 12;
    } else if (status == dev::eth::TransactionException::OutOfStack)
    {
        index = 13;
    } else if (status == dev::eth::TransactionException::StackUnderflow)
    {
        index = 14;
    } else if (status == dev::eth::TransactionException::CreateWithValue)
    {
        index = 15;
    }
    auto it = exceptionMap.find(index);
    if (it != exceptionMap.end())
    {
        return it->first;
    } else
    {
        return 0;
    }
}

string GetExceptedInfo(uint32_t index)
{
    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (!IsEnabled)
    {
        return "";
    }
    auto it = exceptionMap.find(index);
    if (it != exceptionMap.end())
    {
        return it->second;
    } else
    {
        return "";
    }
}
extern UniValue blockToJSON(const CBlock& block, const CBlockIndex* blockindex, bool txDetails);

bool ContractTxConnectBlock(CTransaction tx, uint32_t transactionIndex, CCoinsViewCache *v,
                            const CBlock &block,
                            int nHeight,
                            ByteCodeExecResult &bcer,
                            bool bLogEvents,
                            bool fJustCheck,
                            std::map<dev::Address, std::pair<CHeightTxIndexKey, std::vector<uint256>>> &heightIndexes,
                            int &level, string &errinfo,uint64_t &countCumulativeGasUsed,uint64_t &blockGasUsed)
{
    CBlockIndex* pblockindex = chainActive.Tip();

    UniValue blockJson = blockToJSON(block, pblockindex, true);

    //LogPrintf("Block hex: %s\n", block.ToString());
    //LogPrintf("Block json: %s\n", blockJson.write(UniValue::VOBJ));

    if (!block.IsContractEnabled())
    {
        return false;
    }

    uint64_t minGasPrice = GetMinGasPrice(nHeight + 1);
    uint64_t blockGasLimit = GetBlockGasLimit(nHeight + 1);

    // LogPrintf("before GkcTxConverter: vtx addr %p\n", &block.vtx); //gkc debug
    //LogPrintf("before GkcTxConverter: vtx size %d\n", block.vtx.size()); //gkc debug

    GkcTxConverter convert(tx, v, &block.vtx);

    ExtractGkcTX resultConvertQtumTX;
    //LogPrintf("ContractTxConnectBlock: vtx addr %p\n", &block.vtx); //gkc debug
    //LogPrintf("ContractTxConnectBlock: vtx size %d\n", block.vtx.size()); //gkc debug
    if (!convert.extractionGkcTransactions(resultConvertQtumTX))
    {
        level = 100;
        errinfo = "bad-tx-bad-contract-format";
        return false;
    }
    if (!CheckMinGasPrice(resultConvertQtumTX.second, minGasPrice))
    {
        level = 100;
        errinfo = "bad-tx-low-gas-price";
        return false;
    }

    dev::u256 gasAllTxs = dev::u256(0);
    LogPrintf("ContractTxConnectBlock() : before ByteCodeExec\n");
    ByteCodeExec exec(block, resultConvertQtumTX.first, blockGasLimit);
    LogPrintf("ContractTxConnectBlock() : after ByteCodeExec\n");
    //validate VM version and other ETH params before execution
    //Reject anything unknown (could be changed later by DGP)
    //TODO evaluate if this should be relaxed for soft-fork purposes
    bool nonZeroVersion = false;
    dev::u256 sumGas = dev::u256(0);
    CAmount nTxFee = v->GetValueIn(tx) - tx.GetValueOut();
    for (GkcTransaction &qtx : resultConvertQtumTX.first)
    {
        sumGas += qtx.gas() * qtx.gasPrice();

        if (sumGas > dev::u256(INT64_MAX))
        {
            level = 100;
            errinfo = "bad-tx-gas-stipend-overflow";
            return false;
        }

        if (sumGas > dev::u256(nTxFee))
        {
            level = 100;
            errinfo = "bad-txns-fee-notenough";
            return false;
        }

        VersionVM v = qtx.getVersion();
        if (v.format != 0)
        {
            level = 100;
            errinfo = "bad-tx-version-format";
            return false;
        }
        if (v.rootVM != 0)
        {
            nonZeroVersion = true;
        } else
        {
            if (nonZeroVersion)
            {
                //If an output is version 0, then do not allow any other versions in the same tx
                level = 100;
                errinfo = "bad-tx-mixed-zero-versions";
                return false;
            }
        }
        if (!(v.rootVM == 0 || v.rootVM == 1))
        {
            level = 100;
            errinfo = "bad-tx-version-rootvm";
            return false;
        }
        if (v.vmVersion != 0)
        {
            level = 100;
            errinfo = "bad-tx-version-vmversion";
            return false;
        }
        if (v.flagOptions != 0)
        {
            level = 100;
            errinfo = "bad-tx-version-flags";
            return false;
        }
        //check gas limit is not less than minimum gas limit (unless it is a no-exec tx)
        if (qtx.gas() < MINIMUM_GAS_LIMIT && v.rootVM != 0)
        {
            level = 100;
            errinfo = "bad-tx-too-little-gas";
            return false;
        }
        if (qtx.gas() > UINT32_MAX)
        {
            level = 100;
            errinfo = "bad-tx-too-much-gas";
            return false;
        }
        gasAllTxs += qtx.gas();
        if (gasAllTxs > dev::u256(blockGasLimit))
        {
            level = 1;
            errinfo = "bad-txns-gas-exceeds-blockgaslimit";
            return false;
        }
        //don't allow less than DGP set minimum gas price to prevent MPoS greedy mining/spammers
        if (v.rootVM != 0 && (uint64_t)qtx.gasPrice() < minGasPrice)
        {
            level = 100;
            errinfo = "bad-tx-low-gas-price";
            return false;
        }
    }

    if (!nonZeroVersion)
    {
        //if tx is 0 version, then the tx must already have been added by a previous contract execution
        if (!tx.HasOpSpend())
        {
            level = 100;
            errinfo = "bad-tx-improper-version-0";
            return false;
        }
    }

    LogPrintf("ContractTxConnectBlock() : before exec.performByteCode\n");
    if (!exec.performByteCode(dev::eth::Permanence::Committed,nHeight))
    {LogPrintf("ContractTxConnectBlock() : after exec.performByteCode fail\n");
        level = 100;
        errinfo = "bad-tx-unknown-error";
        return false;
    }LogPrintf("ContractTxConnectBlock() : after exec.performByteCode ok\n");

    std::vector<ResultExecute> resultExec(exec.getResult());
    if (!exec.processingResults(bcer))
    {
        level = 100;
        errinfo = "bad-vm-exec-processing";
        return false;
    }

    countCumulativeGasUsed += bcer.usedGas;
    std::vector<TransactionReceiptInfo> tri;
    if (bLogEvents)
    {
        for (size_t k = 0; k < resultConvertQtumTX.first.size(); k++)
        {
            dev::Address key = resultExec[k].execRes.newAddress;
            if (!heightIndexes.count(key))
            {
                heightIndexes[key].first = CHeightTxIndexKey(nHeight, resultExec[k].execRes.newAddress);
            }
            heightIndexes[key].second.push_back(tx.GetHash());
            uint32_t excepted = GetExcepted(resultExec[k].execRes.excepted);
            tri.push_back(
                        TransactionReceiptInfo{block.GetHash(), uint32_t(nHeight), tx.GetHash(), uint32_t(transactionIndex),
                                               resultConvertQtumTX.first[k].from(), resultConvertQtumTX.first[k].to(),
                                               countCumulativeGasUsed, uint64_t(resultExec[k].execRes.gasUsed),
                                               resultExec[k].execRes.newAddress, resultExec[k].txRec.log(),
                                               excepted});
        }

        pstorageresult->addResult(uintToh256(tx.GetHash()), tri);
    }

    blockGasUsed += bcer.usedGas;
    if (blockGasUsed > blockGasLimit)
    {
        level = 1000;
        errinfo = "bad-blk-gaslimit";
        return false;
    }

    //gkc-vm
    if (fRecordLogOpcodes && !fJustCheck)
    {
        writeVMlog(resultExec, tx, block);
    }

    for (ResultExecute &re: resultExec)
    {
        if (re.execRes.newAddress != dev::Address() && !fJustCheck){
            LogPrintf("Address : %s :", std::string(re.execRes.newAddress.hex()));
        }
    }
    return true;
}

void GetState(uint256 &hashStateRoot, uint256 &hashUTXORoot)
{
	assert(globalState);
    dev::h256 oldHashStateRoot(globalState->rootHash()); // gkc-vm
    dev::h256 oldHashUTXORoot(globalState->rootHashUTXO()); // gkc-vm

    hashStateRoot = h256Touint(oldHashStateRoot);
    hashUTXORoot = h256Touint(oldHashUTXORoot);
}

void UpdateState(uint256 hashStateRoot, uint256 hashUTXORoot)
{
    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();

    if (hashStateRoot.IsNull() || hashUTXORoot.IsNull())
    {
        return;
    }
    globalState->setRoot(uintToh256(hashStateRoot));
    globalState->setRootUTXO(uintToh256(hashUTXORoot));
}

void DeleteResults(std::vector<CTransaction> const &txs)
{
    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (!IsEnabled)
    {
        return;
    }
    pstorageresult->deleteResults(txs);
}

std::vector<TransactionReceiptInfo> GetResult(uint256 const &hashTx)
{
    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (!IsEnabled)
    {
        return std::vector<TransactionReceiptInfo>();
    }
    return pstorageresult->getResult(uintToh256(hashTx));
}

void CommitResults()
{
    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (!IsEnabled)
    {
        return;
    }
    pstorageresult->commitResults();
}

void ClearCacheResult()
{
    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (!IsEnabled)
    {
        return;
    }
    pstorageresult->clearCacheResult();
}

std::map<dev::h256, std::pair<dev::u256, dev::u256>> GetStorageByAddress(string address)
{
    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (!IsEnabled)
    {
        return std::map<dev::h256, std::pair<dev::u256, dev::u256>>();
    }

    dev::Address addrAccount(address);
    auto storage(globalState->storage(addrAccount));
    return storage;
};

void SetTemporaryState(uint256 hashStateRoot, uint256 hashUTXORoot)
{

    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (!IsEnabled)
    {
        return;
    }
    if (hashStateRoot.IsNull() || hashUTXORoot.IsNull())
    {
        return;
    }
    TemporaryState ts(globalState);
    ts.SetRoot(uintToh256(hashStateRoot), uintToh256(hashUTXORoot));
}

std::unordered_map<dev::h160, dev::u256> GetContractList()
{
    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (!IsEnabled)
    {
        return std::unordered_map<dev::h160, dev::u256>();
    }
    auto map = globalState->addresses();
    return map;
};


CAmount GetContractBalance(dev::h160 address)
{
    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (!IsEnabled)
    {
        return CAmount(0);
    }
    return CAmount(globalState->balance(address));
}

std::vector<uint8_t> GetContractCode(dev::Address address)
{
    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (!IsEnabled)
    {
        return std::vector<uint8_t>();
    }
    return globalState->code(address);
}

bool GetContractVin(dev::Address address, dev::h256 &hash, uint32_t &nVout, dev::u256 &value,
                    uint8_t &alive)
{
    bool ret = false;
    bool IsEnabled =  chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (!IsEnabled)
    {
        return ret;
    }
    std::unordered_map<dev::Address, Vin> vins = globalState->vins();
    if (vins.count(address))
    {
        hash = vins[address].hash;
        nVout = vins[address].nVout;
        value = vins[address].value;
        alive = vins[address].alive;
        ret = true;
    }
    return ret;
}

UniValue executionResultToJSON(const dev::eth::ExecutionResult &exRes)
{
    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("gasUsed", CAmount(exRes.gasUsed)));
    std::stringstream ss;
    ss << exRes.excepted;
    result.push_back(Pair("excepted", ss.str()));
    result.push_back(Pair("newAddress", exRes.newAddress.hex()));
    result.push_back(Pair("output", HexStr(exRes.output)));
    result.push_back(Pair("codeDeposit", static_cast<int32_t>(exRes.codeDeposit)));
    result.push_back(Pair("gasRefunded", CAmount(exRes.gasRefunded)));
    result.push_back(Pair("depositSize", static_cast<int32_t>(exRes.depositSize)));
    result.push_back(Pair("gasForDeposit", CAmount(exRes.gasForDeposit)));
    return result;
}

UniValue transactionReceiptToJSON(const dev::eth::TransactionReceipt &txRec)
{
    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("stateRoot", txRec.stateRoot().hex()));
    result.push_back(Pair("gasUsed", CAmount(txRec.cumulativeGasUsed())));
    result.push_back(Pair("bloom", txRec.bloom().hex()));
    UniValue logEntries(UniValue::VARR);
    dev::eth::LogEntries logs = txRec.log();
    for (dev::eth::LogEntry log : logs)
    {
        UniValue logEntrie(UniValue::VOBJ);
        logEntrie.push_back(Pair("address", log.address.hex()));
        UniValue topics(UniValue::VARR);
        for (dev::h256 l : log.topics)
        {
            topics.push_back(l.hex());
        }
        logEntrie.push_back(Pair("topics", topics));
        logEntrie.push_back(Pair("data", HexStr(log.data)));
        logEntries.push_back(logEntrie);
    }
    result.push_back(Pair("log", logEntries));
    return result;
}

void RPCCallContract(UniValue &result, const string addrContract, std::vector<unsigned char> opcode,
                     string sender, uint64_t gasLimit)
{
    bool IsEnabled = chainActive.Tip() && chainActive.Tip()->IsContractEnabled();
    if (!IsEnabled)
    {
        return;
    }

    dev::Address addrAccount(addrContract);
    dev::Address senderAddress(sender);

    std::vector<ResultExecute> execResults = CallContract(addrAccount, opcode, senderAddress, gasLimit);
    if (fRecordLogOpcodes)
    {
        writeVMlog(execResults);
    }
    result.push_back(Pair("executionResult", executionResultToJSON(execResults[0].execRes)));
    result.push_back(Pair("transactionReceipt", transactionReceiptToJSON(execResults[0].txRec)));
}

LastHashes::LastHashes()
{}

void LastHashes::set(const CBlockIndex *tip)
{
    clear();

    m_lastHashes.resize(256);
    for(int i=0;i<256;i++){
        if(!tip)
            break;
        m_lastHashes[i]= uintToh256(*tip->phashBlock);
        tip = tip->pprev;
    }
}

dev::h256s LastHashes::precedingHashes(const dev::h256 &) const
{
    return m_lastHashes;
}

void LastHashes::clear()
{
    m_lastHashes.clear();
}


dev::eth::EnvInfo ByteCodeExec::BuildEVMEnvironment(int nHeight){
    dev::u256 gasUsed;
    dev::eth::BlockHeader header;
    
    CBlockIndex* tip = chainActive.Tip();
    
    if (0 == nHeight || NULL == chainActive[nHeight]) {
        header.setNumber(tip->nHeight + 1);
        LogPrintf("*****1\n");
    } else {
        tip = chainActive[nHeight];
        header.setNumber(nHeight);
        LogPrintf("*****2   nHeight:d%\n",nHeight);
    }
    header.setTimestamp(block.nTime);
    header.setDifficulty(dev::u256(block.nBits));
    header.setGasLimit(blockGasLimit);

    lastHashes.set(tip);

    LogPrintf("*****BuildEVMEnvironment  block.nTime:%d\n", block.nTime);
    LogPrintf("*****BuildEVMEnvironment  block.nBits:%d\n", block.nBits);

    LogPrintf("*****BuildEVMEnvironment  tip.nHeight:%ld\n", tip->nHeight);
    LogPrintf("*****BuildEVMEnvironment  blockGasLimit:%ld\n", blockGasLimit);

    if(block.IsProofOfStake()){
        if (block.GetBlockHeader().nVersion < SMART_CONTRACT_VERSION)
            header.setAuthor(EthAddrFromScript(block.vtx[1].vout[1].scriptPubKey));
        else
            header.setAuthor(EthAddrFromScript(block.vtx[1].vout[2].scriptPubKey));
    }else {
        header.setAuthor(EthAddrFromScript(block.vtx[0].vout[0].scriptPubKey));
    }

    dev::eth::EnvInfo env(header, lastHashes, gasUsed);
    
    return env;
}



bool ByteCodeExec::performByteCode(dev::eth::Permanence type, int nHeight)
{
    for (GkcTransaction &tx : txs)
    {
        //LogPrintf("performByteCode() : validate VM version\n");
        //validate VM version
        if (tx.getVersion().toRaw() != VersionVM::GetEVMDefault().toRaw())
        {
            //LogPrintf("performByteCode() : validate VM version failed\n");
            return false;
        }
        //LogPrintf("performByteCode() : validate VM version ok\n");

        dev::eth::EnvInfo envInfo(BuildEVMEnvironment(nHeight));

        std::unique_ptr<dev::eth::SealEngineFace> se(dev::eth::ChainParams(dev::eth::genesisInfo(dev::eth::Network::HomesteadTest)).createSealEngine());


        if (!tx.isCreation() && !globalState->addressInUse(tx.receiveAddress()))
        {
           // LogPrintStr("performByteCode execption=====\n"); //gkc debug
            dev::eth::ExecutionResult execRes;
            execRes.excepted = dev::eth::TransactionException::Unknown;
            result.push_back(ResultExecute{execRes, dev::eth::TransactionReceipt(dev::h256(), dev::u256(),
                                           dev::eth::LogEntries()),
                                           CTransaction()});
            continue;
        }
        //LogPrintStr("performByteCode start exec=====\n"); //gkc debug

        //LogPrintf("*****performByteCode stateRoot: %s, utxoRoot: %s\n", h256Touint(globalState->rootHashUTXO()).GetHex().c_str(), h256Touint(globalState->rootHash()).GetHex().c_str());

        ResultExecute res_ = globalState->execute(envInfo, *se.get(), tx, type, dev::eth::OnOpFunc());


        //LogPrintf("*****ResultExecute tx:%s\n",res_.tx.ToString()); //gkc debug

        result.push_back(res_);
    }
    globalState->db().commit();
    globalState->dbUtxo().commit();
    //globalSealEngine.get()->deleteAddresses.clear();
    return true;
}

static CTxOut CreateRefundTxOut(const CAmount& amount, CScript& script)
{
	CTxOut refund(amount, script);
	refund.SetType(CTxOut::Type::GAS_REFUND);
	return refund;
}

bool ByteCodeExec::processingResults(ByteCodeExecResult &resultBCE)
{
    for (size_t i = 0; i < result.size(); i++)
    {
        uint64_t gasUsed = (uint64_t)result[i].execRes.gasUsed;
        if (result[i].execRes.excepted != dev::eth::TransactionException::None)
        {
            //LogPrintStr("TransactionException != None"); //gkc debug
            if (txs[i].value() > 0)
            { //refund the value to sender
                CMutableTransaction tx;
                tx.vin.push_back(CTxIn(h256Touint(txs[i].getHashWith()), txs[i].getNVout(), CScript() << OP_SPEND));
                CScript script(CScript() << OP_DUP << OP_HASH160 << txs[i].sender().asBytes() << OP_EQUALVERIFY
                               << OP_CHECKSIG);
                tx.vout.push_back(CreateRefundTxOut(CAmount(txs[i].value()), script));
                resultBCE.valueTransfers.push_back(CTransaction(tx));

               // LogPrintf("*****222:: gas[%d]:%ld\n",i,CAmount(txs[i].value())); //gkc debug

            }
            resultBCE.usedGas += gasUsed;



        } else
        {



            if (txs[i].gas() > UINT64_MAX ||
                    result[i].execRes.gasUsed > UINT64_MAX ||
                    txs[i].gasPrice() > UINT64_MAX)
            {
                return false;
            }
            uint64_t gas = (uint64_t)txs[i].gas();
            uint64_t gasPrice = (uint64_t)txs[i].gasPrice();

            resultBCE.usedGas += gasUsed;
            int64_t amount = (gas - gasUsed) * gasPrice;

//            LogPrintf("*****:: gasUsed[%d]:%ld\n",i,gasUsed); //gkc debug

//            LogPrintf("*****:: gas[%d]:%ld\n",i,gas); //gkc debug
//            LogPrintf("*****:: gasPrice[%d]:%ld\n",i,gasPrice); //gkc debug
//            LogPrintf("*****:: amount[%d]:%ld\n",i,amount); //gkc debug


            if (amount < 0)
            {
                return false;
            }
            if (amount > 0)
            {
                CScript script(CScript() << OP_DUP << OP_HASH160 << txs[i].sender().asBytes() << OP_EQUALVERIFY
                               << OP_CHECKSIG);
                resultBCE.refundOutputs.push_back(CreateRefundTxOut(amount, script));
                resultBCE.refundSender += amount;
            }
        }
        if (result[i].tx != CTransaction())
        {
            LogPrint("processingResults ","%d", i); //gkc debug
            resultBCE.valueTransfers.push_back(result[i].tx);
        }
    }
    return true;
}


dev::Address ByteCodeExec::EthAddrFromScript(const CScript &script)
{
    CTxDestination addressBit;
    txnouttype txType = TX_NONSTANDARD;
    if (ExtractDestination(script, addressBit, &txType))
    {
        if ((txType == TX_PUBKEY || txType == TX_PUBKEYHASH) &&
                addressBit.type() == typeid(CKeyID))
        {
            CKeyID addressKey(boost::get<CKeyID>(addressBit));
            std::vector<unsigned char> addr(addressKey.begin(), addressKey.end());
            LogPrint("ByteCodeExec::EthAddrFromScript ","%s", HexStr(addr.begin(), addr.end())); //gkc debug
            return dev::Address(addr);
        }
    }
    //if not standard or not a pubkey or pubkeyhash output, then return 0
    return dev::Address();
}


bool GkcTxConverter::extractionGkcTransactions(ExtractGkcTX &gkctx)
{
    std::vector<GkcTransaction> resultTX;
    std::vector<EthTransactionParams> resultETP;
    for (size_t i = 0; i < txBit.vout.size(); i++)
    {
        if (txBit.vout[i].scriptPubKey.HasOpCreate() || txBit.vout[i].scriptPubKey.HasOpCall())
        {
            if (receiveStack(txBit.vout[i].scriptPubKey))
            {
                EthTransactionParams params;
                if (parseEthTXParams(params))
                {
                    LogPrintStr("extractionGkcTransactions\n"); //gkc debug
                    resultTX.push_back(createEthTX(params, i));
                    resultETP.push_back(params);
                    LogPrintStr("createEthTX and push back\n");
                } else
                {LogPrintf("parseEthTXParams return failed \n");
                    return false;
                }
            } else
            {LogPrintf("receiveStack return failed \n");
                return false;
            }
        }
    }
    gkctx = std::make_pair(resultTX, resultETP);
    return true;
}

bool GkcTxConverter::receiveStack(const CScript &scriptPubKey)
{
    EvalScript(stack, scriptPubKey, SCRIPT_EXEC_BYTE_CODE, BaseSignatureChecker(),  nullptr);
    LogPrintf("receiveStack stack.empty(): %d \n", stack.empty());
    if (stack.empty())
        return false;

    CScript scriptRest(stack.back().begin(), stack.back().end());
    stack.pop_back();

    opcode = (opcodetype)(*scriptRest.begin());
    LogPrintf("receiveStack stack.size(): %d \n", stack.size());
    if ((opcode == OP_CREATE && stack.size() < 4) || (opcode == OP_CALL && stack.size() < 5))
    {
        stack.clear();
        return false;
    }

    return true;
}

bool GkcTxConverter::parseEthTXParams(EthTransactionParams &params)
{
    try
    {
		const bool isVersion2 = (stack.size() == 7);
		if(isVersion2)
		{
			stack.pop_back(); //senderSignature
			stack.pop_back(); //senderPubKey
		}
		
        dev::Address receiveAddress;
        valtype vecAddr;
        if (opcode == OP_CALL)
        {
            vecAddr = stack.back();
            stack.pop_back();
            receiveAddress = dev::Address(vecAddr);
        }
        if (stack.size() < 4)
            return false;

        if (stack.back().size() < 1)
        {
            return false;
        }
        valtype code(stack.back());
        stack.pop_back();
        uint64_t gasPrice = CScriptNum::vch_to_uint64(stack.back());
        stack.pop_back();
        uint64_t gasLimit = CScriptNum::vch_to_uint64(stack.back());
        stack.pop_back();
        if (gasPrice > INT64_MAX || gasLimit > INT64_MAX)
        {
            return false;
        }
        //we track this as CAmount in some places, which is an int64_t, so constrain to INT64_MAX
        if (gasPrice != 0 && gasLimit > INT64_MAX / gasPrice)
        {
            //overflows past 64bits, reject this tx
            return false;
        }
        if (stack.back().size() > 4)
        {
            return false;
        }
        VersionVM version = VersionVM::fromRaw((uint32_t)CScriptNum::vch_to_uint64(stack.back()));
        stack.pop_back();
        params.version = version;
        params.gasPrice = dev::u256(gasPrice);
        params.receiveAddress = receiveAddress;
        params.code = code;
        params.gasLimit = dev::u256(gasLimit);
        return true;
    }
    catch (const scriptnum_error &err)
    {
        LogPrintStr("Incorrect parameters to VM.");
        return false;
    }
}

GkcTransaction GkcTxConverter::createEthTX(const EthTransactionParams &etp, uint32_t nOut)
{
    GkcTransaction txEth;LogPrintStr("createEthTX txEth\n");
    if (etp.receiveAddress == dev::Address() && opcode != OP_CALL)
    {
        txEth = GkcTransaction(txBit.vout[nOut].nValue, etp.gasPrice, etp.gasLimit, etp.code, dev::u256(0));
    } else
    {
        txEth = GkcTransaction(txBit.vout[nOut].nValue, etp.gasPrice, etp.gasLimit, etp.receiveAddress, etp.code,
                                dev::u256(0));
    }
    LogPrintStr("createEthTX before GetSenderAddress\n");
    valtype types = GetSenderAddress(txBit, view, blockTransactions);
    LogPrintStr("createEthTX after GetSenderAddress, type: \n");
    dev::Address sender(GetSenderAddress(txBit, view, blockTransactions));
    txEth.forceSender(sender);
    txEth.setHashWith(uintToh256(txBit.GetHash()));
    txEth.setNVout(nOut);
    txEth.setVersion(etp.version);

    return txEth;
}



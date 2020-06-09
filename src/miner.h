// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_MINER_H
#define BITCOIN_MINER_H

#include <stdint.h>
#include "primitives/transaction.h"
#include "primitives/block.h"

class CBlock;
class CBlockHeader;
class CBlockIndex;
class CReserveKey;
class CScript;
class CWallet;

struct CBlockTemplate;

static const int32_t BYTECODE_TIME_BUFFER = 6;

/** Run the miner threads */
void GenerateBitcoins(bool fGenerate, CWallet* pwallet, int nThreads);
/** Generate a new block, without valid proof-of-work */
CBlockTemplate* CreateNewPowBlock(CBlockIndex* pindexPrev, CWallet* pwallet);
CBlockTemplate* CreateNewBlock(const CScript& scriptPubKeyIn, CWallet* pwallet, bool fProofOfStake);
CBlockTemplate* CreateNewBlockWithKey(CReserveKey& reservekey, CWallet* pwallet, bool fProofOfStake);
/** Modify the extranonce in a block */
void IncrementExtraNonce(CBlock* pblock, CBlockIndex* pindexPrev, unsigned int& nExtraNonce);
/** Check mined block */
void UpdateTime(CBlockHeader* block, const CBlockIndex* pindexPrev);

void BitcoinMiner(CWallet* pwallet, bool fProofOfStake);

void RebuildRefundTransaction(CBlock *pblock, CAmount &nFees);
bool AttemptToAddContractToBlock(const CTransaction &iter, uint64_t minGasPrice, CBlockTemplate *pblockTemplate, uint64_t &nBlockSize, int &nBlockSigOps, uint64_t &nBlockTx, CCoinsViewCache &view, CAmount &nFees);

void FillPowBlockPayee(BlockHeight newHeight, CAmount fees, std::vector<CTxOut>& vout);
void FillPosBlockPayee(BlockHeight newHeight, BLOCK_VERSION, CAmount stakeInputAmount, std::vector<CTxOut>& vout);
void FillStakeMinerVout(BlockHeight newHeight, BLOCK_VERSION blockVersion, CAmount minerReward, CAmount stakeInputAmount, std::vector<CTxOut>& vout);

extern double dHashesPerSec;
extern int64_t nHPSTimerStart;
extern bool fMinePosBlock;

#endif // BITCOIN_MINER_H

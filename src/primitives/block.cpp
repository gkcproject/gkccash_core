// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2015-2017 The PIVX developers
// Copyright (c) 2017-2018 The GKC developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "primitives/block.h"

#include "hash.h"
#include "script/standard.h"
#include "script/sign.h"
#include "tinyformat.h"
#include "utilstrencodings.h"
#include "util.h"

#include "crypto/common.h"
#include "crypto/scrypt.h"

uint256 CBlockHeader::GetHash() const
{
    if(nVersion < ZEROCOIN_VERSION)
        return HashQuark(BEGIN(nVersion), END(nNonce));

    return HashQuark(BEGIN(nVersion), END(nAccumulatorCheckpoint));
}

uint256 CBlockHeader::GetPoWHash() const
{
    uint256 scryptHash;
//    uint256 headerHash;
    
    scrypt_1024_1_1_256(BEGIN(nVersion), BEGIN(scryptHash));
    
//    scrypt(BEGIN(scryptHash), scryptHash.size(), BEGIN(scryptHash), scryptHash.size(), BEGIN(headerHash), 1024, 1, 1, headerHash.size());
    
    return HashQuark(BEGIN(scryptHash),END(scryptHash));
}

void CBlock::SetNull()
{
	CBlockHeader::SetNull();
	vtx.clear();
	vMerkleTree.clear();
	payee = CScript();
	vchBlockSig.clear();
}

uint256 CBlock::BuildMerkleTree(bool* fMutated) const
{
    /* WARNING! If you're reading this because you're learning about crypto
       and/or designing a new system that will use merkle trees, keep in mind
       that the following merkle tree algorithm has a serious flaw related to
       duplicate txids, resulting in a vulnerability (CVE-2012-2459).

       The reason is that if the number of hashes in the list at a given time
       is odd, the last one is duplicated before computing the next level (which
       is unusual in Merkle trees). This results in certain sequences of
       transactions leading to the same merkle root. For example, these two
       trees:

                    A               A
                  /  \            /   \
                B     C         B       C
               / \    |        / \     / \
              D   E   F       D   E   F   F
             / \ / \ / \     / \ / \ / \ / \
             1 2 3 4 5 6     1 2 3 4 5 6 5 6

       for transaction lists [1,2,3,4,5,6] and [1,2,3,4,5,6,5,6] (where 5 and
       6 are repeated) result in the same root hash A (because the hash of both
       of (F) and (F,F) is C).

       The vulnerability results from being able to send a block with such a
       transaction list, with the same merkle root, and the same block hash as
       the original without duplication, resulting in failed validation. If the
       receiving node proceeds to mark that block as permanently invalid
       however, it will fail to accept further unmodified (and thus potentially
       valid) versions of the same block. We defend against this by detecting
       the case where we would hash two identical hashes at the end of the list
       together, and treating that identically to the block having an invalid
       merkle root. Assuming no double-SHA256 collisions, this will detect all
       known ways of changing the transactions without affecting the merkle
       root.
    */
    vMerkleTree.clear();
    vMerkleTree.reserve(vtx.size() * 2 + 16); // Safe upper bound for the number of total nodes.
    for (std::vector<CTransaction>::const_iterator it(vtx.begin()); it != vtx.end(); ++it)
        vMerkleTree.push_back(it->GetHash());
    int j = 0;
    bool mutated = false;
    for (int nSize = vtx.size(); nSize > 1; nSize = (nSize + 1) / 2)
    {
        for (int i = 0; i < nSize; i += 2)
        {
            int i2 = std::min(i+1, nSize-1);
            if (i2 == i + 1 && i2 + 1 == nSize && vMerkleTree[j+i] == vMerkleTree[j+i2]) {
                // Two identical hashes at the end of the list at a particular level.
                mutated = true;
            }
            vMerkleTree.push_back(Hash(BEGIN(vMerkleTree[j+i]),  END(vMerkleTree[j+i]),
                                       BEGIN(vMerkleTree[j+i2]), END(vMerkleTree[j+i2])));
        }
        j += nSize;
    }
    if (fMutated) {
        *fMutated = mutated;
    }
    return (vMerkleTree.empty() ? uint256() : vMerkleTree.back());
}

std::vector<uint256> CBlock::GetMerkleBranch(int nIndex) const
{
    if (vMerkleTree.empty())
        BuildMerkleTree();
    std::vector<uint256> vMerkleBranch;
    int j = 0;
    for (int nSize = vtx.size(); nSize > 1; nSize = (nSize + 1) / 2)
    {
        int i = std::min(nIndex^1, nSize-1);
        vMerkleBranch.push_back(vMerkleTree[j+i]);
        nIndex >>= 1;
        j += nSize;
    }
    return vMerkleBranch;
}

uint256 CBlock::CheckMerkleBranch(uint256 hash, const std::vector<uint256>& vMerkleBranch, int nIndex)
{
    if (nIndex == -1)
		return uint256();
    for (std::vector<uint256>::const_iterator it(vMerkleBranch.begin()); it != vMerkleBranch.end(); ++it)
    {
        if (nIndex & 1)
            hash = Hash(BEGIN(*it), END(*it), BEGIN(hash), END(hash));
        else
            hash = Hash(BEGIN(hash), END(hash), BEGIN(*it), END(*it));
        nIndex >>= 1;
    }
    return hash;
}

std::string CBlock::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=%d, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%u)\n",
        GetHash().ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce,
        vtx.size());
    for (unsigned int i = 0; i < vtx.size(); i++)
    {
        s << "  " << vtx[i].ToString() << "\n";
    }
    s << "  vMerkleTree: ";
    for (unsigned int i = 0; i < vMerkleTree.size(); i++)
        s << " " << vMerkleTree[i].ToString();
    s << "\n";
    return s.str();
}

void CBlock::print() const
{
    LogPrintf("%s", ToString());
}

// ppcoin: sign block
bool CBlock::SignBlock(const CKeyStore& keystore)
{
    std::vector<valtype> vSolutions;
    txnouttype whichType;

    if(!IsProofOfStake())
    {
        for(unsigned int i = 0; i < vtx[0].vout.size(); i++)
        {
            const CTxOut& txout = vtx[0].vout[i];

            if (!Solver(txout.scriptPubKey, whichType, vSolutions))
                continue;

            if (whichType == TX_PUBKEY)
            {
                // Sign
                CKeyID keyID;
                keyID = CKeyID(uint160(vSolutions[0]));

                CKey key;
                if (!keystore.GetKey(keyID, key))
                    return false;

                //vector<unsigned char> vchSig;
                if (!key.Sign(GetHash(), vchBlockSig))
                     return false;

                return true;
            }
        }
    }
    else
    {
        const CTxOut& txout = (nVersion < SMART_CONTRACT_VERSION) ? vtx[1].vout[1] : vtx[1].vout[2];

        if (!Solver(txout.scriptPubKey, whichType, vSolutions)) {
            LogPrintf("Solver failed: %s\n", txout.scriptPubKey.ToString().c_str());
            return false;
        }

        if (whichType == TX_PUBKEYHASH)
        {

            CKeyID keyID;
            keyID = CKeyID(uint160(vSolutions[0]));

            CKey key;
            if (!keystore.GetKey(keyID, key)) {
                LogPrintf("SignBlock TX_PUBKEYHASH GetKey failed: %s\n", txout.scriptPubKey.ToString().c_str());
                return false;
            }

            //vector<unsigned char> vchSig;
            if (!key.Sign(GetHash(), vchBlockSig)) {
                 LogPrintf("SignBlock TX_PUBKEYHASH Sign failed: \n");
                 return false;
            }

            return true;

        }
        else if(whichType == TX_PUBKEY)
        {
            CKeyID keyID;
            keyID = CPubKey(vSolutions[0]).GetID();
            CKey key;
            if (!keystore.GetKey(keyID, key)) {
                LogPrintf("TX_PUBKEY GetKey failed: \n");
                return false;
            }

            //vector<unsigned char> vchSig;
            if (!key.Sign(GetHash(), vchBlockSig)) {
                LogPrintf("TX_PUBKEY Sign failed: \n");
                return false;
            }

            return true;
        }
    }

    LogPrintf("Sign failed\n");
    return false;
}

bool CBlock::CheckBlockSignature() const
{
    if (IsProofOfWork())
        return vchBlockSig.empty();

    std::vector<valtype> vSolutions;
    txnouttype whichType;

    const CTxOut& txout = (nVersion < SMART_CONTRACT_VERSION) ? vtx[1].vout[1] : vtx[1].vout[2];

    if (!Solver(txout.scriptPubKey, whichType, vSolutions))
        return false;

    if (whichType == TX_PUBKEY)
    {
        valtype& vchPubKey = vSolutions[0];
        CPubKey pubkey(vchPubKey);
        if (!pubkey.IsValid())
          return false;

        if (vchBlockSig.empty())
            return false;

        return pubkey.Verify(GetHash(), vchBlockSig);
    }
    else if(whichType == TX_PUBKEYHASH)
    {
        valtype& vchPubKey = vSolutions[0];
        CKeyID keyID;
        keyID = CKeyID(uint160(vchPubKey));
        CPubKey pubkey(vchPubKey);

        if (!pubkey.IsValid())
          return false;

        if (vchBlockSig.empty())
            return false;

        return pubkey.Verify(GetHash(), vchBlockSig);

    }

    return false;
}

VM_STATE_ROOT CBlock::GetVMState(uint256 &hashStateRoot, uint256 &hashUTXORoot) const
{
    if (this->nVersion <= ZEROCOIN_VERSION)
        return RET_CONTRACT_UNENBALE;

    assert(vtx.size() > 1);
    const CTransaction &tx = vtx[1];  // 0
    assert(tx.IsCoinBase2() == true);

    int index = 0;
    unsigned int  i = 0;
    for (i = 0; i < tx.vout.size(); i++)
    {
        if (tx.vout[i].scriptPubKey.HasOpVmHashState())
        {
            index = i;
            break;
        }
    }

    if(i >= tx.vout.size())
    {  // must to have VmHashState vout
        assert(0);
        LogPrintStr("Error: GetVMState coinbase vout");
        return RET_VM_STATE_ERR;
    }
    // have VmHashState vout
    std::vector<std::vector<unsigned char> > stack;
    EvalScript(stack, tx.vout[index].scriptPubKey, SCRIPT_EXEC_BYTE_CODE, BaseSignatureChecker(),
               nullptr);
    if (stack.empty())
    {
        // VmHashState vout script err
        assert(0);
        LogPrintStr("Error: GetVMState coinbase vout.scriptPubKey err");
        return RET_VM_STATE_ERR;
    }

    std::vector<unsigned char> code(stack.back());
    stack.pop_back();

    std::vector<unsigned char> vechashUTXORoot(stack.back());
    stack.pop_back();

    std::string strUTXO;
    strUTXO = HexStr(vechashUTXORoot);
    hashUTXORoot = uint256S(strUTXO);

    std::vector<unsigned char> vechashStateRoot(stack.back());

    stack.pop_back();

    std::string strHASH;
    strHASH = HexStr(vechashStateRoot);
    hashStateRoot = uint256S(strHASH);
    return RET_VM_STATE_OK;

}

const CTransaction* CBlock::GetCoinStake() const
{
	return IsProofOfStake() ? &vtx[1] : nullptr;
}
CTransaction* CBlock::GetCoinStake()
{
	return IsProofOfStake() ? &vtx[1] : nullptr;
}
AgentID CBlock::GetAgentId() const
{
	const CTransaction* coinstake = GetCoinStake();
	if(!coinstake)
		return AgentID();
	return coinstake->agentid;
}


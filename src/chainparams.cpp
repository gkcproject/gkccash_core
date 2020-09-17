// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "libzerocoin/Params.h"
#include "chainparams.h"
#include "random.h"
#include "util.h"
#include "utilstrencodings.h"
#include "main.h"
#include "forkheights.h" // forkheight_release_v_2_5_1, forkheight_release_v_2_4_0

#include <assert.h>

#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

/**
 * Main network
 */

//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress>& vSeedsOut, const SeedSpec6* data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7 * 24 * 60 * 60;
    for (unsigned int i = 0; i < count; i++) {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

//   What makes a good checkpoint block?
// + Is surrounded by blocks with reasonable timestamps
//   (no blocks before with a timestamp after, none after with
//    timestamp before)
// + Contains no strange transactions
static Checkpoints::MapCheckpoints mapCheckpoints =
    boost::assign::map_list_of
	(1,uint256("f47369b56a1270fc3cfef891a56cadc58a71ca31eeaf248d2bc09d18988561c6"))
	(2,uint256("f83e7421a041d378a022ef5f814a6bb25fda09b7dcfd93cd3b88964606f5edc0"))
	(3,uint256("62c4ad40e6f0e623ea965d9517d5b49fac1220e313a3914344fc1b9edfd4f318"))
	(10,uint256("ba8c06c5e28057e6ad6075a94f8e9dd78e40577028f3e84d509a6c7e48000efe"))
	(31,uint256("2a0abd66cc04d882aef0b4c61f1368a1a315685a17d2cd9984adc94f9509c05a"))
	(51,uint256("dde2be3b2026774eb0322ed2a95e460ad311051b27b3a04f04d8c326b59a9243"))
	(100,uint256("ed5d6820c73a00c223f38d7ce03b4e5926bff01af4adfac3f010299104ee1443"))
	(1000,uint256("026285cea15bf370196d6cf30e5c349617fdb720aa9fde7f8f1239ab768cd885"))
	(10000,uint256("4708949a50d3f698bb6eb29ae4469591217e62e3c77115a3481a360a9f26a876"))
	(20000,uint256("d3f5d29b5d6741877a56810b65c5eb28395afa9a721c693d47b8b803a07d174e"))
	(30000,uint256("d061d48849dde4538c89101eae92704a8488f4ae313d1d1f47bc5ac6c9b8d807"))
	(30500,uint256("72e70207193799b629a70d081356310f1455d28f33efc632fb4e4e0a572e5a76"))
	(31000,uint256("e8d9894ae91c98d53c8674064db9ac7a6d7f9bfdc7eb19d30d905ba36e710163"))
	(31500,uint256("e4e5dde61095114ed0cb9e4134478bdf5a6dfc9596198290aeb79eecb691434d"))
	(31695,uint256("826f31916eadd2e752e6141a0f228e846a14fadd94613bd5e8d147390bad8f48"))
	(32695,uint256("b5742da48629eb1cbcda5aad7df570d566d68b28f848856ae872ebea25ac3555"))
	(33695,uint256("4eaaac66be37971c4058740f43bbcb1a98519223e606771e75490d9141b45275"))
	(33972,uint256("701589dcb5085cbc6df83cd75fb7860b72947e6d3f88129bb73fe5e31a66c5d0"))
	(34695,uint256("bb6e4e6ad1e64ef98ef2804142620e829dbcc9af4f6492412b894d01d509c3ab"))
	(35695,uint256("7ef730b6dcbcf38b88e5d34675cadd0f287dc41e6c69d40a83e4bcf9b97361f4"))
	(36800,uint256("a7327dc502c7981e7a55607898b394b771f12b7c1a20c03f3720c6ecf9ebb6cc"))
	(36900,uint256("580079f9c6bd3883897bb5446e4b0d856d21864c9ab18cfcf4592c03275f4866"))
	(37000,uint256("1e6cd3d8dc0b3a1a9c8fe6155cc7c58204cd5cd92e00d58819ecc460e596c3ab"))
	(37050,uint256("9c27405d20bb6545e70ff97a46fedd74485e082ab9371344efa1868d11c59f01"))
	(204389,uint256("a8194dd30f1bed4ac02538c32ef8b330022cbb93b68e39d44c721a2720eee197"))
	(207282,uint256("663ebd87401399b891e6a63e692a71d2f99ee4bd0fd4508c6a3dc03cc5aa8d79"));

static const Checkpoints::CCheckpointData data = {
    &mapCheckpoints,
    1510948627, // * UNIX timestamp of last checkpoint block
    0,          // * total number of transactions between genesis and last checkpoint
                //   (the tx=... number in the SetBestChain debug.log lines)
    2000        // * estimated number of transactions per day after checkpoint
};

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
    boost::assign::map_list_of(0, uint256("0x001"));
static const Checkpoints::CCheckpointData dataTestnet = {
    &mapCheckpointsTestnet,
    1740710,
    0,
    250};

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
    boost::assign::map_list_of(0, uint256("0x001"));
static const Checkpoints::CCheckpointData dataRegtest = {
    &mapCheckpointsRegtest,
    1454124731,
    0,
    100};

libzerocoin::ZerocoinParams* CChainParams::Zerocoin_Params() const
{
    assert(this);
    static CBigNum bnHexModulus = 0;
    if (!bnHexModulus)
        bnHexModulus.SetHex(zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParamsHex = libzerocoin::ZerocoinParams(bnHexModulus);

    return &ZCParamsHex;
}

CChainParams::CChainParams()
{
	forkheight_checkPubkeyAddress = forkheight_release_v_2_4_0;
	forkheight_lockDepriveTx = forkheight_release_v_2_4_0;
	forkheight_increaseMaturity = forkheight_release_v_2_5_1;
	forkheight_cancelLockDepriveTx = forkheight_release_v_2_5_1;
}

class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";
        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0xef;
        pchMessageStart[1] = 0xec;
        pchMessageStart[2] = 0xf5;
        pchMessageStart[3] = 0xe5;
        vAlertPubKey = ParseHex("0490e0480bf864eece4ddca8787bb1a74f823361e7e9e931e67385b6976600a98637e2f2eb32568035266789c44e0471443bbe27efd6673284e53d4f16272566f9");
        nDefaultPort = 38802;
        bnProofOfWorkLimit = ~uint256(0) >> 0;
        bnProofOfStakeLimit = ~uint256(0) >> 24;
        bnProofOfStakeLimit_V2 = ~uint256(0) >> 20;
        nMaxReorganizationDepth = 100;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 0;
        nMaturity = 6;
		nMaturity_v2 = 100;
        nMasternodeCountDrift = 20;
        nMaxMoneyOut = 21148191990 * COIN;

        nModifierUpdateBlock = 0;
        nZerocoinStartTime = 1560237828;    // August 10, 2018 08:00:00 AM

        // Activation height for TimeProtocolV2, Blocks V7 and newMessageSignatures
        nBlockTimeProtocolV2 = 400; // !TODO: change me

        /**
         * Build the genesis block. Note that the output of the genesis coinbase cannot
         * be spent as it did not originally exist in the database.
         *
         * CBlock(hash=00000ffd590b14, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=e0028e, nTime=1390095618, nBits=1e0ffff0, nNonce=28917698, vtx=1)
         *   CTransaction(hash=e0028e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
         *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d01044c5957697265642030392f4a616e2f3230313420546865204772616e64204578706572696d656e7420476f6573204c6976653a204f76657273746f636b2e636f6d204973204e6f7720416363657074696e6720426974636f696e73)
         *     CTxOut(nValue=50.00000000, scriptPubKey=0xA9037BAC7050C479B121CF)
         *   vMerkleTree: e0028e
         */
        const char* pszTimestamp = "Recreate the Future.";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 503382015 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 250 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04c10e83b2703ccf322f7dbd62dd5855ac7c10bd055814ce121ba32607d573b8810c02c0582aed05b4deb9c4b77b26d92428c61256cd42774babea0a073b2ed0c9") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = GENESIS_VERSION;
        genesis.nTime = 1560237828;
        genesis.nBits = 0x1f090786;
        genesis.nNonce = 5517;

        hashGenesisBlock = genesis.GetHash();



		// 修改块结构后，打开以下注释块，重新计算创世块hash和hashMerkleRoot
//        bool fNegative;
//        bool fOverflow;
//        uint256 bnTarget;
//
//
//        bnTarget.SetCompact(genesis.nBits, &fNegative, &fOverflow);
//
//
//        printf("new mainnet bnTarget: %s\n", bnTarget.ToString().c_str());
//        printf("new mainnet bnProofOfWorkLimit: %s\n", bnProofOfWorkLimit.ToString().c_str());
//
//
//
//        {
//            printf("bnProofOfWorkLimit:  %s \n", bnProofOfWorkLimit.GetHex().c_str());
//            printf("CMainParams recalculating params for mainnet.\n");
//            printf("old mainnet genesis nonce:  %u \n", genesis.nNonce);
//            printf("old mainnet genesis hash:  %s\n", hashGenesisBlock.ToString().c_str());                    // deliberately empty for loop finds nonce value.
//                    for(genesis.nNonce = 0; genesis.GetPoWHash() > bnTarget; genesis.nNonce++)
//            {
//                      if(genesis.nNonce % 1000 == 0)
//                           printf("Still tring:%u \n",genesis.nNonce);
//            }
//            printf("new mainnet genesis merkle root: %s\n", genesis.hashMerkleRoot.ToString().c_str());
//            printf("new mainnet genesis nonce:  %u \n", genesis.nNonce);
//            printf("new mainnet genesis hash: %s\n", genesis.GetHash().ToString().c_str());
//
//        }


		assert(hashGenesisBlock == uint256("0f55d306d2c054aa60171fe68ec2c7690d5212445798a7408e079c491645b83c"));
		assert(genesis.hashMerkleRoot == uint256("75c851e6bb5122f23aec361030ec33ff33eb60d214ce2945a7fc797cbb7d4e9d"));

		vSeeds.push_back(CDNSSeedData("seed0.gogkc.com","seed0.gogkc.com"));
		vSeeds.push_back(CDNSSeedData("seed1.gogkc.com","seed1.gogkc.com"));
		vSeeds.push_back(CDNSSeedData("seed2.gogkc.com","seed2.gogkc.com"));
		vSeeds.push_back(CDNSSeedData("seed3.gogkc.com","seed3.gogkc.com"));
		vSeeds.push_back(CDNSSeedData("seed4.gogkc.com","seed4.gogkc.com"));
		vSeeds.push_back(CDNSSeedData("seed5.gogkc.com","seed5.gogkc.com"));
		vSeeds.push_back(CDNSSeedData("seed6.gogkc.com","seed6.gogkc.com"));
		vSeeds.push_back(CDNSSeedData("seed7.gogkc.com","seed7.gogkc.com"));
		vSeeds.push_back(CDNSSeedData("seed8.gogkc.com","seed8.gogkc.com"));
		vSeeds.push_back(CDNSSeedData("seed9.gogkc.com","seed9.gogkc.com"));
		vSeeds.push_back(CDNSSeedData("seed10.gogkc.com","seed10.gogkc.com"));
		vSeeds.push_back(CDNSSeedData("seed01.gogkc.cash","seed01.gogkc.cash"));
		vSeeds.push_back(CDNSSeedData("seed02.gogkc.cash","seed02.gogkc.cash"));
		vSeeds.push_back(CDNSSeedData("seed03.gogkc.cash","seed03.gogkc.cash"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 38); // 38 -> 'G', refer to: https://en.bitcoin.it/wiki/List_of_address_prefixes
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 5);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 128);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();
        // 	BIP44 coin type is from https://github.com/satoshilabs/slips/blob/master/slip-0044.md
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x77).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = false;
        fTestnetToBeDeprecatedFieldRPC = false;
        fHeadersFirstSyncingActive = false;

        nPoolMaxTransactions = 3;
        strSporkKey = "04f868526d10d4a734e155853f76b4a8be21b941a6891dfbd0f0a5889adf7fcb052d843acddd2af776f6834b71e6c53778eabcecc357a9578ae70d276257fd8b80";
        strObfuscationPoolDummyAddress = "UXJdBngPh9maq59nbyazqptD8qacGJb7ak";
        nStartMasternodePayments = 1514768399; //Wed, 25 Jun 2018 8:59:59 GMT

        /** Zerocoin */
        zerocoinModulus = "b024c540851a3461c17a8da2b39752f5b6f59cd753c0de054a8b2f8b19c2729ef5c5fc99ebc4507d7926934fac50219ff55639316c1e278f4972188ea8bc3bae"
            "fcc0b4f747023b52cbbc8ff71a2e065a7ddd40b86b79753b9f8fd707abed76598c1bbcfa98f14be0655cfd68049f56a6579f0f839d3c0a8a4bc09fb2b70eb310"
            "65512b9935cc1bb76d1ac05d4c61517606ca4c7ddc05d17ec5aa8b7551c5f1c4bca7e6e29ee79515eab5b13d49ba94acb71eab013ca27d346d04e7f814b44b71"
            "0bad17e2551fd48b57260cc481e5e1be765bcdb0e62dc8c76742e5807e6fff8258ece6e84c3b1da8a26ddd1e72c40e6cee46d72d97435cfc8c1fb4feaa67d847"
            "2f56fece6f78dba68ea00e9d1c5c48640f22e2c73511a47f91fd77d11c5f1b411e26e4b25696192ba44bbf567cb43b60b5d312087fe76ad6f9cacd66a58223cf"
            "1f1340f559335a2d0dcca3c8021c3351";
        nMaxZerocoinSpendsPerTransaction = 7; // Assume about 20kb each
        nMinZerocoinMintFee = 1 * CENT; //high fee required for zerocoin mints
        nMintRequiredConfirmations = 20; //the maximum amount of confirmations until accumulated in 19
        nRequiredAccumulation = 1;
        nDefaultSecurityLevel = 100; //full security level for accumulators
        //nZerocoinHeaderVersion = 4; //Block headers must be this version once zerocoin is active
        nBudget_Fee_Confirmations = 6; // Number of confirmations for the finalization fee

		nMnemonicBlock = 1;
		forkheight_clearInactiveUser = forkheight_release_v_2_4_0;
		forkheight_modifyAdPrice = forkheight_release_v_2_4_0; // blockheight in MAINNET
		forkheight_addJoinHeight = 352800;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return data;
    }
};
static CMainParams mainParams;

/*
assert height: pow < pos < zero < contract
see block version setting in CreateNewBlock()
*/

int CChainParams::Zerocoin_StartHeight() const { 
	// Fix me: How to query POS plans based on different chain heights
	return crp::CoinReleasePlan::GetInstance().GetPosPlan(0).heightRange.Begin() + 1;
}
int CChainParams::Contract_StartHeight() const {
	return Zerocoin_StartHeight() + 1;
}
int64_t CChainParams::TargetSpacing() const{
	return crp::CoinReleasePlan::GetInstance().pow.velocity;
}

int CChainParams::COINBASE_MATURITY(BlockHeight chainHeight) const {
	int n = nMaturity;
	if(chainHeight >= forkheight_increaseMaturity)
		n = nMaturity_v2;
	return n;
}

bool CChainParams::IsTimeProtocolV2(const int nHeight) const {
	return nHeight >= nBlockTimeProtocolV2;
}

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
        pchMessageStart[0] = 0xcf;
        pchMessageStart[1] = 0xcc;
        pchMessageStart[2] = 0xd5;
        pchMessageStart[3] = 0xc5;
        vAlertPubKey = ParseHex("04146d06bd90bf85bec61fb050e0016dc6fd9b078f86ae6f8a2281a861065d40de1cf02788c0a2324e1020cd13bc323d8bdb66e1bf24d241cf601d347f1aa3ad78");
        nDefaultPort = 38804;
        bnProofOfWorkLimit = ~uint256(0) >> 8; // GKC starting difficulty is 1 / 2^16
        nSubsidyHalvingInterval = 210000;
        nMaxReorganizationDepth = 100;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 0;
        bckContractAddress = "6e323704df5fc687c4f39b980315a8f8f740c971";
        nMaturity = 20;
        nMasternodeCountDrift = 20;
        nModifierUpdateBlock = 0; //approx Mon, 17 Apr 2017 04:00:00 GMT
        nMaxMoneyOut = 21148191990 * COIN;
        nZerocoinStartTime = 1560237828;

        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1560237828;
        genesis.nNonce = 4922032;

        hashGenesisBlock = genesis.GetHash();


//                bool fNegative;
//                bool fOverflow;
//                uint256 bnTarget;
//
//
//                bnTarget.SetCompact(genesis.nBits, &fNegative, &fOverflow);
//
//
//                printf("new mainnet bnTarget: %s\n", bnTarget.ToString().c_str());
//                printf("new mainnet bnProofOfWorkLimit: %s\n", bnProofOfWorkLimit.ToString().c_str());
//
//
//
//                {
//                    printf("bnProofOfWorkLimit:  %s \n", bnProofOfWorkLimit.ToString().c_str());
//                    printf("CMainParams recalculating params for testnet.\n");
//                            for(genesis.nNonce == 0; genesis.GetPoWHash() > bnTarget; genesis.nNonce++)
//                    {
//                              if(genesis.nNonce % 1000 == 0)
//                                   printf("Still tring:%u \n",genesis.nNonce);
//                    }
//                    printf("new mainnet genesis merkle root: %s\n", genesis.hashMerkleRoot.ToString().c_str());
//                    printf("new mainnet genesis nonce:  %u \n", genesis.nNonce);
//                    printf("new mainnet genesis hash: %s\n", genesis.GetHash().ToString().c_str());
//
//                }


        assert(hashGenesisBlock == uint256("0xdd410742600479ea7eeabeab05f16d56fbb3c11470b577a6c46efdbfcedd1018"));






        vFixedSeeds.clear();
        vSeeds.clear();

//        vSeeds.push_back(CDNSSeedData("45.32.77.35", "45.32.77.35"));               // US node address
//        vSeeds.push_back(CDNSSeedData("139.180.147.249", "139.180.147.249"));       // US node address
//        vSeeds.push_back(CDNSSeedData("199.247.18.33", "199.247.18.33"));           // Canada node address
//        vSeeds.push_back(CDNSSeedData("47.111.65.209", "47.111.65.209"));           // CN node address

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 97); // 97 -> 'g', refer to: https://en.bitcoin.it/wiki/List_of_address_prefixes
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 196);  // Testnet gkc script addresses start with '8' or '9'
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 239);     // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        // Testnet gkc BIP32 pubkeys start with 'DRKV'
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        // Testnet gkc BIP32 prvkeys start with 'DRKP'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
        // Testnet gkc BIP44 coin type is '1' (All coin's testnet default)
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x01).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        nPoolMaxTransactions = 2;
        strSporkKey = "042a5bab8709d72839c05e72a6396b69608a164abe94eeccdc311a9e0dd8fc8e3a13d75bcd2ffa6077ffea53319fa0e60ac0ba0bc6b392f7f296f0164d6ff08250";
        strObfuscationPoolDummyAddress = "uTxyz1tQzBkZrJaZgFQ9TXn2TxsSQQcoQ8";
        nStartMasternodePayments = 1514768399; //Fri, 09 Jan 2015 21:05:58 GMT
        nBudget_Fee_Confirmations = 3; // Number of confirmations for the finalization fee. We have to make this very short
                                       // here because we only have a 8 block finalization window on testnet
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams
{
public:
    CRegTestParams()
    {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        strNetworkID = "regtest";
        pchMessageStart[0] = 0xef;
        pchMessageStart[1] = 0xec;
        pchMessageStart[2] = 0xf5;
        pchMessageStart[3] = 0xc5;
        nSubsidyHalvingInterval = 150;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        genesis.nTime = 1560237828;
        genesis.nBits = 0x1f00ffff;
        genesis.nNonce = 2;

        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 38806;


//        {
//            printf("bnProofOfWorkLimit:  %u \n", bnProofOfWorkLimit);
//            printf("CMainParams recalculating params for mainnet.\n");
//            printf("old mainnet genesis nonce:  %u \n", genesis.nNonce);
//            printf("old mainnet genesis hash:  %s\n", hashGenesisBlock.ToString().c_str());                    // deliberately empty for loop finds nonce value.
//                    for(genesis.nNonce == 0; genesis.GetPoWHash() > bnProofOfWorkLimit; genesis.nNonce++)
//            {
//                      if(genesis.nNonce % 1000 == 0)
//                           printf("Still tring:%u \n",genesis.nNonce);
//            }
//            printf("new mainnet genesis merkle root: %s\n", genesis.hashMerkleRoot.ToString().c_str());
//            printf("new mainnet genesis nonce:  %u \n", genesis.nNonce);
//            printf("new mainnet genesis hash2: %s\n", genesis.GetHash().ToString().c_str());

//        }


        //assert(hashGenesisBlock == uint256("0x6a3da50c0306a2ba83c41465e8c3fd3e4fa44a1d6ac7c672adb607c10324795a"));

        vFixedSeeds.clear(); //! Testnet mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Testnet mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataRegtest;
    }
};
static CRegTestParams regTestParams;

/**
 * Unit test
 */
class CUnitTestParams : public CMainParams, public CModifiableParams
{
public:
    CUnitTestParams()
    {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 58808;
        vFixedSeeds.clear(); //! Unit test mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Unit test mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fAllowMinDifficultyBlocks = false;
        fMineBlocksOnDemand = true;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        // UnitTest share the same checkpoints as MAIN
        return data;
    }

    //! Published setters to allow changing values in unit test cases
    virtual void setSubsidyHalvingInterval(int anSubsidyHalvingInterval) { nSubsidyHalvingInterval = anSubsidyHalvingInterval; }
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority) { nEnforceBlockUpgradeMajority = anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority) { nRejectBlockOutdatedMajority = anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority) { nToCheckBlockUpgradeMajority = anToCheckBlockUpgradeMajority; }
    virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks) { fDefaultConsistencyChecks = afDefaultConsistencyChecks; }
    virtual void setAllowMinDifficultyBlocks(bool afAllowMinDifficultyBlocks) { fAllowMinDifficultyBlocks = afAllowMinDifficultyBlocks; }
    virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};
static CUnitTestParams unitTestParams;


static CChainParams* pCurrentParams = 0;

CModifiableParams* ModifiableParams()
{
    assert(pCurrentParams);
    assert(pCurrentParams == &unitTestParams);
    return (CModifiableParams*)&unitTestParams;
}

const CChainParams& Params()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(CBaseChainParams::Network network)
{
    switch (network) {
    case CBaseChainParams::MAIN:
        return mainParams;
    case CBaseChainParams::TESTNET:
        return testNetParams;
    case CBaseChainParams::REGTEST:
        return regTestParams;
    case CBaseChainParams::UNITTEST:
        return unitTestParams;
    default:
        assert(false && "Unimplemented network");
        return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}

const uint256& CChainParams::ProofOfStakeLimit(const bool fV2) const
{ 
	return fV2 ? bnProofOfStakeLimit_V2 : bnProofOfStakeLimit;
}


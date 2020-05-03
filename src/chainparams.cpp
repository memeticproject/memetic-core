// Copyright (c) 2014 BlackCoin Developers
// Copyright (c) 2014 DashCoin Developers
// Copyright (c) 2015 Transfercoin Developer
// Copyright (c) 2015-2016 PepeCoin Developers
// Copyright (c) 2016-2019 The PepeCoin Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
 
#include "assert.h"
 
#include "chainparams.h"
#include "main.h"
#include "util.h"
 
#include <boost/assign/list_of.hpp>
 
using namespace boost::assign;
 
struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};
 
#include "chainparamsseeds.h"
 
//
// Main network
//
 
// Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress> &vSeedsOut, const SeedSpec6 *data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7*24*60*60;
    for (unsigned int i = 0; i < count; i++)
    {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}
 
class CMainParams : public CChainParams {
public:
    CMainParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0x3a;
        pchMessageStart[1] = 0xc4;
        pchMessageStart[2] = 0x2c;
        pchMessageStart[3] = 0x2f;
        vAlertPubKey = ParseHex("0x0451d55bb3330f555fed011b93a04a9d00ec542dd805c80b1babd7e29bca08dd7fc796cb822dfd20087421a5dc21c0a2fdd6dbb5c82229f41eb9f57e4a076dd4ed");
        nDefaultPort = 29377;
        nRPCPort = 29376;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 16);
 
        const char* pszTimestamp = "March 5, 2016: UN human rights chief backs Apple in FBI encryption row - BBC News";
        std::vector<CTxIn> vin;
        vin.resize(1);
        vin[0].scriptSig = CScript() << 0 << CBigNum(42) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        std::vector<CTxOut> vout;
        vout.resize(1);
        vout[0].SetEmpty();
        CTransaction txNew(1, 1457184666, vin, vout, 0);
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime    = 1457184666;
        genesis.nBits    = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce   = 663024;
 
        hashGenesisBlock = genesis.GetHash();
/*
		if (false ) {
		// This will figure out a valid hash and Nonce if you're
		// creating a different genesis block:
	    uint256 hashTarget = CBigNum().SetCompact(genesis.nBits).getuint256();
	    while (genesis.GetHash() > hashTarget)
	    {
	        ++genesis.nNonce;
	        if (genesis.nNonce == 0)
		        {
		            printf("NONCE WRAPPED, incrementing time");
		            ++genesis.nTime;
		        }
	         }
	   }
 
        //// debug print
	      printf("block.GetHash() == %s\n", genesis.GetHash().ToString().c_str());
	      printf("block.hashMerkleRoot == %s\n", genesis.hashMerkleRoot.ToString().c_str());
	      printf("block.nTime = %u \n", genesis.nTime);
	      printf("block.nNonce = %u \n", genesis.nNonce);
*/
        assert(hashGenesisBlock == uint256("0x00008cae6a01358d774087e2daf3b2108252b0b5a440195ffec4fd38f9892272"));
        assert(genesis.hashMerkleRoot == uint256("0xf8d91b2c93bd7222515770b67f20d3380edf8292548daeb3b0ce82fd5764dcac"));
 
 
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,55); // pepecoin addresses start with P
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,85);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,153);
        base58Prefixes[KEKDAQ_SECRET_KEY] =     std::vector<unsigned char>(1,99);
        base58Prefixes[STEALTH_ADDRESS] = std::vector<unsigned char>(1,40);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();;
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();;
 
        vSeeds.push_back(CDNSSeedData("seed.kekdaq.com",  "seed.kekdaq.com"));
	vSeeds.push_back(CDNSSeedData("seed.pepecoin.net",  "seed.pepecoin.net"));
	vSeeds.push_back(CDNSSeedData("seed.memetic.ai",  "seed.memetic.ai"));
 
        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));
 
        nPoolMaxTransactions = 3;
        //strSporkKey = "046f78dcf911fbd61910136f7f0f8d90578f68d0b3ac973b5040fb7afb501b5939f39b108b0569dca71488f5bbf498d92e4d1194f6f941307ffd95f75e76869f0e";
        //strMasternodePaymentsPubKey = "046f78dcf911fbd61910136f7f0f8d90578f68d0b3ac973b5040fb7afb501b5939f39b108b0569dca71488f5bbf498d92e4d1194f6f941307ffd95f75e76869f0e";
        strDarksendPoolDummyAddress = "PJJBpDg5LKJhAkqGDhG4Wuo9AUXBkz4SWY";
        nLastPOWBlock = 71489;  //change from 70000 to 71489 and increase protocol version to resolve the fork issue
        nPOSStartBlock = 9000;
        nRestartPOWBlock = 320000; // when POW restarts
    }
 
    virtual const CBlock& GenesisBlock() const { return genesis; }
    virtual Network NetworkID() const { return CChainParams::MAIN; }
 
    virtual const vector<CAddress>& FixedSeeds() const {
        return vFixedSeeds;
    }
protected:
    CBlock genesis;
    vector<CAddress> vFixedSeeds;
};
static CMainParams mainParams;
 
 
//
// Testnet
//
 
class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0x2b;
        pchMessageStart[1] = 0xca;
        pchMessageStart[2] = 0x3c;
        pchMessageStart[3] = 0x3f;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 16);
        vAlertPubKey = ParseHex("04cc24ab003c828cdd9cf4db2ebbde8e1cecb3bbfa8b3127fcb9dd9b84d44112080827ed7c49a648af9fe788ff42e316aee665879c553f099e55299d6b54edd7e0");
        nDefaultPort = 39377;
        nRPCPort = 39376;
        strDataDir = "testnet";
 
        // Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nBits  = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce = 663024;
 
        assert(hashGenesisBlock == uint256("0x00008cae6a01358d774087e2daf3b2108252b0b5a440195ffec4fd38f9892272"));
 
        vFixedSeeds.clear();
        vSeeds.clear();
 
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,55);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY]     = std::vector<unsigned char>(1,239);
        base58Prefixes[KEKDAQ_SECRET_KEY]     = std::vector<unsigned char>(1,63);
        base58Prefixes[STEALTH_ADDRESS] = std::vector<unsigned char>(1,40);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();;
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();;
 
        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));
 
        nLastPOWBlock = 5000000;
        nPOSStartBlock = 100;
        // nRestartPOWBlock = 320000; // when POW restarts
    }
    virtual Network NetworkID() const { return CChainParams::TESTNET; }
};
static CTestNetParams testNetParams;
 
 
static CChainParams *pCurrentParams = &mainParams;
 
const CChainParams &Params() {
    return *pCurrentParams;
}
 
void SelectParams(CChainParams::Network network) {
    switch (network) {
        case CChainParams::MAIN:
            pCurrentParams = &mainParams;
            break;
        case CChainParams::TESTNET:
            pCurrentParams = &testNetParams;
            break;
        default:
            assert(false && "Unimplemented network");
            return;
    }
}
 
bool SelectParamsFromCommandLine() {
 
    bool fTestNet = GetBoolArg("-testnet", false);
 
    if (fTestNet) {
        SelectParams(CChainParams::TESTNET);
    } else {
        SelectParams(CChainParams::MAIN);
    }
    return true;
}

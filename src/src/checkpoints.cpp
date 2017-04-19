// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 100;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactionsg
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (0,	Params().HashGenesisBlock() )
        (14, uint256("0xeb1a88f40f84c13d061a4ededd50f03977cb815f459b44f0acf8bbc3593cd57e"))
        (38, uint256("0xdb2420ff33b119de3bf0eda27c3e4653aa13a63b21009fb2412b3e45dce9a136"))
        (776, uint256("0x19fae8ae0a8c77a239ab8551b57f02cf0ac584a6eaab2b4cc9195745839b24b6"))
        (1863, uint256("0xb9ca005de3783c7be996fc95901394dd08a2213e480015f32ef671e9ed47c7b1"))
        (2578, uint256("0x2984f39527006fe0206a0ae76717f7f193b3007e61cb39e9befab3902c74bb2b"))
        (4381, uint256("0x2e52f5d4bd18f4a413697dfd4913728bfb2a2f680377d7096f31c1399df0ece7"))
        (6347, uint256("0x256b0d65bf31f0a1a2cdaefae07e1b832b8bfe339248af070338a85880cf6f8d"))
        (9139, uint256("0x01d892847b2c609ea2ff06c804fc36cf784da590ca5657a0661970ddc2ff61d2"))
        (13782, uint256("0xe0be7d54614fb55329e8a0a9d2865da0ccca54d032fbc30d3a551644d8be9292"))
        (15420, uint256("0x2a592c35d8b7e26a8d642f2b3e27fb586884b5f12fee6f52b09a1fea5edf2fa1"))
        (19909, uint256("0xf9c933d941b39698dea5078bd5270cc4af0af11b6302c51ca87cf2a13c1cafdc"))
        (24938, uint256("0xb33bd09e56e696e5e0547845c9646d2cefc04ef25e1b90bb9018d9fb89f788da"))
        (28308, uint256("0x77b1e4d7a29655dc33c38d32fa833407f5ad59c7dee76fcc78231387e7311360"))
        (32000, uint256("0x17f909ef335c27bf44655377622927122c5b6824815a867d52cd31d5aaac2873"))
        (48398, uint256("0xdfe376d27cf8c6f11beaa29f4a3966f1011847b9af91ff81fbe69fc03178294b"))
        (71320, uint256("0x4298a1d06c0effb8a6f15205bb5e6d7c6ea3d6ef68a16a40ef9480c49a3cf212"))
        (219777, uint256("0xe9f0929405524b5a3b2888c392351c902a11b7a167b3e12d63720b62d344230f"))
        (333777, uint256("0x45bff45918baa567e50568a5e0218e9796b9ef8ea3bdb44e1e6c17f95e687542"))
        (447777, uint256("0x307f3cb162219d6763439851f4c81ae6d0b867adccc97c6cc8d962b67f0de43a"))
        (549333, uint256("0xfd66916698a7cb627b8187be3852b9df59d23a73e57d2e03eeee2da06a068bed"))        
    ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();
        if (nHeight <= pindexSync->nHeight){
            return false;
        }
        return true;
    }
}
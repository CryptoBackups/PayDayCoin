// Copyright (c) 2018 PayDayCoin
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "core.h"
#include "txrewardpool.h"
#include "main.h" // for CTransaction

using namespace std;

CTxRewardPool::CTxRewardPool()
{
}

bool CTxRewardPool::add(const uint256& hash, CTransaction &tx)
{
    // Add to memory pool without checking anything.
    // Used by main.cpp AcceptToMemoryPool(), which DOES do
    // all the appropriate checks.
    LOCK(cs);
    {
        mapRTx[hash] = tx;
        for (unsigned int i = 0; i < tx.vin.size(); i++)
            mapNextRTx[tx.vin[i].prevout] = CInPoint(&mapRTx[hash], i);

    }
    return true;
}

bool CTxRewardPool::remove(const CTransaction &tx, bool fRecursive)
{
    // Remove transaction from memory pool
    {
        LOCK(cs);
        uint256 hash = tx.GetHash();
        if (mapRTx.count(hash))
        {
            if (fRecursive) {
                for (unsigned int i = 0; i < tx.vout.size(); i++) {
                    std::map<COutPoint, CInPoint>::iterator it = mapNextRTx.find(COutPoint(hash, i));
                    if (it != mapNextRTx.end())
                        remove(*it->second.ptx, true);
                }
            }
            BOOST_FOREACH(const CTxIn& txin, tx.vin)
                mapNextRTx.erase(txin.prevout);
            mapRTx.erase(hash);
        }
    }
    return true;
}

void CTxRewardPool::clear()
{
    LOCK(cs);
    mapRTx.clear();
    mapNextRTx.clear();
}

bool CTxRewardPool::lookup(uint256 hash, CTransaction& result) const
{
    LOCK(cs);
    std::map<uint256, CTransaction>::const_iterator i = mapRTx.find(hash);
    if (i == mapRTx.end()) return false;
    result = i->second;
    return true;
}

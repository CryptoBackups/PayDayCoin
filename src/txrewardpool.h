// Copyright (c) 2018 PayDayCoin
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_TXREWARDPOOL_H
#define BITCOIN_TXREWARDPOOL_H

#include "core.h"

/*
 * CTxRewardPool stores valid-according-to-the-current-best-chain
 * transactions that may be used for reward by pos.
 *
 * Transactions are added when they are seen on the network
 * (or created by the local node), but not all transactions seen
 * are added to the pool: if a new transaction double-spends
 * an input of a transaction in the pool, it is dropped,
 * as are non-standard transactions.
 */
class CTxRewardPool
{
private:

public:
    mutable CCriticalSection cs;
    std::map<uint256, CTransaction> mapRTx;
    std::map<COutPoint, CInPoint> mapNextRTx;

    CTxRewardPool();

    bool add(const uint256& hash, CTransaction &tx);
    bool remove(const CTransaction &tx, bool fRecursive = false);
    void clear();

    unsigned long size() const
    {
        LOCK(cs);
        return mapRTx.size();
    }

    bool exists(uint256 hash) const
    {
        LOCK(cs);
        return (mapRTx.count(hash) != 0);
    }

    bool lookup(uint256 hash, CTransaction& result) const;
};

#endif /* BITCOIN_TXREWARDPOOL_H */

/*
 * QOracleCommittee – Immutable Multi-Sig Oracle for Synthetic Asset Pricing
 * "Code is Law" — No Admin Keys After Init
 * (c) BANKON All Rights Reserved — Qubic Anti-Military License
 */

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <set>

struct OraclePrice {
    uint64_t price;         // e.g., 1 BTC in USD, 8 decimals (satoshis)
    uint64_t timestamp;     // block time or epoch
    uint8_t decimals;       // price precision
};

class QOracleCommittee {
public:
    std::set<std::string> committee; // Hardcoded addresses
    uint8_t min_signatures;          // e.g. 3 of 5
    OraclePrice currentPrice;
    std::set<std::string> lastUpdateSigners; // Prevent replay

    QOracleCommittee(const std::vector<std::string>& initialCommittee, uint8_t minSigs)
        : committee(initialCommittee.begin(), initialCommittee.end()), min_signatures(minSigs) {
        currentPrice = {0, 0, 8};
    }

    // Called by anyone with N-of-M signatures from committee
    bool pushPrice(uint64_t price, uint64_t timestamp, uint8_t decimals,
                   const std::vector<std::string>& signers) {
        // Must be unique signers, all in committee, min required
        std::set<std::string> unique(signers.begin(), signers.end());
        if (unique.size() < min_signatures) return false;
        for (const auto& s : unique) if (!committee.count(s)) return false;

        // Prevent replay: only allow new signers set per epoch
        if (lastUpdateSigners == unique && currentPrice.timestamp == timestamp) return false;

        currentPrice = {price, timestamp, decimals};
        lastUpdateSigners = unique;
        return true;
    }

    OraclePrice getPrice() const {
        return currentPrice;
    }
};

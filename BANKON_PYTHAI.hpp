/*
 * BANKON PYTHAI Token – Immutable Fixed Supply
 * "Code is Law" – No Mint After Genesis
 * (c) BANKON All Rights Reserved — Qubic Anti-Military License
 */

#include <cstdint>
#include <string>
#include <unordered_map>

class BANKON_PYTHAI {
    const uint64_t TOTAL_SUPPLY = 100000000000000; // 100,000 * 10^9
    std::unordered_map<std::string, uint64_t> balances;
    bool minted = false;

public:
    // Only deployer can mint once, then no further minting allowed
    bool mint(const std::string& deployer) {
        if (minted) return false;
        balances[deployer] = TOTAL_SUPPLY;
        minted = true;
        return true;
    }
    bool transfer(const std::string& from, const std::string& to, uint64_t amount) {
        if (from == to || balances[from] < amount || amount == 0) return false;
        balances[from] -= amount;
        balances[to] += amount;
        return true;
    }
    uint64_t balanceOf(const std::string& who) const {
        auto it = balances.find(who);
        return it != balances.end() ? it->second : 0;
    }
    uint64_t totalSupply() const { return TOTAL_SUPPLY; }
};

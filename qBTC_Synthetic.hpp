/*
 * qBTC – Qubic Synthetic Bitcoin
 * 1:1 Peg to Oracle Price, Mint/Burn via Oracle
 * (c) BANKON All Rights Reserved — Qubic Anti-Military License
 */

#include <cstdint>
#include <string>
#include <unordered_map>
#include "qOracleCommittee.hpp" // For price oracle

class qBTC {
    std::unordered_map<std::string, uint64_t> balances;
    uint64_t total_supply = 0;
    QOracleCommittee& oracle;

public:
    qBTC(QOracleCommittee& _oracle) : oracle(_oracle) {}

    // Mint qBTC: user deposits value at current price, committee signs off-chain, value/price checked by off-chain scripts.
    bool mint(const std::string& user, uint64_t btc_sats, const std::vector<std::string>& committee_sigs) {
        auto priceInfo = oracle.getPrice();
        if (!oracle.pushPrice(priceInfo.price, priceInfo.timestamp, priceInfo.decimals, committee_sigs)) return false;
        balances[user] += btc_sats;
        total_supply += btc_sats;
        return true;
    }
    // Burn qBTC: user withdraws value at current price (to be implemented with redemption process)
    bool burn(const std::string& user, uint64_t amount) {
        if (balances[user] < amount) return false;
        balances[user] -= amount;
        total_supply -= amount;
        return true;
    }
    uint64_t balanceOf(const std::string& who) const {
        auto it = balances.find(who);
        return it != balances.end() ? it->second : 0;
    }
    uint64_t totalSupply() const { return total_supply; }
    OraclePrice getPrice() const { return oracle.getPrice(); }
};

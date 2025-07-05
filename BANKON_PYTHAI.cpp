/*
 * BANKON PYTHAI Token
 * Code is Law – Qubic Anti-Military License
 * (c) BANKON All Rights Reserved
 */

#include <string>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <sstream>

// ========================== LaunchProtect ==========================
class LaunchProtect {
protected:
    bool initialized = false;
    std::string admin;
    bool burned = false;

    LaunchProtect(const std::string& admin_address)
        : admin(admin_address) {
        emit("LaunchProtectInitialized", admin_address);
    }

    void requireActive(const std::string& sender) const {
        if (!initialized && sender != admin)
            throw std::runtime_error("Not initialized");
        if (burned)
            throw std::runtime_error("Key burned – contract is immutable");
    }

    void requireAdmin(const std::string& sender) const {
        if (sender != admin)
            throw std::runtime_error("Not admin");
        if (burned)
            throw std::runtime_error("Key burned – contract is immutable");
    }

    void finalizeLaunch(const std::string& sender) {
        requireAdmin(sender);
        initialized = true;
        emit("LaunchFinalized", sender);
    }

    void burnKey(const std::string& sender) {
        requireAdmin(sender);
        burned = true;
        admin = "0x000000000000000000000000000000000000dead";
        emit("KeyBurned", sender);
    }

    bool isInitialized() const { return initialized; }
    bool isKeyBurned() const { return burned; }
    std::string adminAddress() const { return admin; }

    void emit(const std::string& event, const std::string& data = "") const {
        std::cout << "[EVENT] " << event << (data.empty() ? "" : ": " + data) << std::endl;
    }
};

// ========================== BANKON PYTHAI Token ==========================
class BankonPythaiToken : public LaunchProtect {
public:
    static constexpr uint64_t DECIMALS = 12;
    static constexpr uint64_t SUPPLY_BASE = 100000; // <-- 100,000 base units
    static constexpr uint64_t TOTAL_SUPPLY = SUPPLY_BASE * 1000000000000ULL; // 100,000 * 10^12
    static const std::string SYMBOL;
    static const std::string NAME;

    // Balances mapping
    std::unordered_map<std::string, uint64_t> balances;

    BankonPythaiToken(const std::string& deployer)
        : LaunchProtect(deployer) 
    {
        // Mint all supply to deployer
        balances[deployer] = TOTAL_SUPPLY;
        emit("Mint", deployer + " minted " + formatAmount(TOTAL_SUPPLY));
    }

    // Standard transfer
    bool transfer(const std::string& sender, const std::string& to, uint64_t amount) {
        requireActive(sender);
        if (sender == to || amount == 0) return false;
        if (balances[sender] < amount) return false;
        balances[sender] -= amount;
        balances[to] += amount;
        emit("Transfer", sender + " -> " + to + " : " + formatAmount(amount));
        return true;
    }

    // Burn tokens (anyone can burn their own)
    bool burn(const std::string& sender, uint64_t amount) {
        requireActive(sender);
        if (balances[sender] < amount || amount == 0) return false;
        balances[sender] -= amount;
        emit("Burn", sender + " burned " + formatAmount(amount));
        return true;
    }

    // View functions
    uint64_t balanceOf(const std::string& addr) const {
        auto it = balances.find(addr);
        return it != balances.end() ? it->second : 0;
    }
    uint64_t totalSupply() const { return TOTAL_SUPPLY; }
    std::string symbol() const { return SYMBOL; }
    std::string name() const { return NAME; }
    uint64_t decimals() const { return DECIMALS; }

private:
    std::string formatAmount(uint64_t amount) const {
        std::ostringstream oss;
        oss << (amount / 1000000000000ULL) << "."
            << std::setw(12) << std::setfill('0') << (amount % 1000000000000ULL);
        return oss.str();
    }
};

// Constants
const std::string BankonPythaiToken::SYMBOL = "BANKON";
const std::string BankonPythaiToken::NAME = "PYTHAI";

// ========================== Qubic Anti-Military License ==========================
/*
Qubic Anti-Military License – Code is Law Edition
Permission is hereby granted, perpetual, worldwide, non-exclusive, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
- The Software cannot be used in any form or in any substantial portions for development, maintenance and for any other purposes, in the military sphere and in relation to military products or activities as defined in the original license.
- All modifications, alterations, or merges must maintain these restrictions.
- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
(c) BANKON All Rights Reserved. See LICENSE file for full text.
*/

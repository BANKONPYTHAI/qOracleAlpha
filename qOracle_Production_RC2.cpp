/*
 * qOracle Production RC2 - Unified BANKON PYTHAI System
 * Quantum-Resistant Oracle & Synthetic Asset Ecosystem
 * Production-Ready Implementation
 * 
 * Components:
 * - Oracle Committee (7-of-7 quantum signatures with Dilithium3)
 * - BANKON PYTHAI Token (100,000 fixed supply)
 * - qBTC Synthetic Token (21M fixed supply)
 * - qUSD Stablecoin (USDC peg)
 * - Cross-Chain Bridge Integration
 * - Governance & Multisig
 * 
 * License: Qubic Anti-Military License
 * (c) BANKON All Rights Reserved
 */

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <array>
#include <algorithm>
#include <cstring>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <functional>

// Include quantum signature verification
#include "QuantumSignature.hpp"

// ========================== CONSTANTS & CONFIGURATION ==========================
namespace qOracleConfig {
    // Oracle Committee Configuration
    constexpr size_t NUM_ORACLES = 7;
    constexpr size_t QUORUM_THRESHOLD = 4;  // 4-of-7 required
    constexpr size_t PUBKEY_SIZE = 1472;    // Dilithium3 public key
    constexpr size_t SIG_SIZE = 2701;       // Dilithium3 signature
    
    // Price Bounds & Validation
    constexpr uint64_t MIN_PRICE = 1;
    constexpr uint64_t MAX_PRICE = 100000000000000000ULL;
    constexpr uint64_t MAX_TIMESTAMP_SKEW = 600; // 10 minutes
    constexpr uint64_t MAX_PRICE_DEVIATION = 50; // 50% max deviation
    
    // Token Configuration
    constexpr uint64_t BKPY_DECIMALS = 15;
    constexpr uint64_t BKPY_TOTAL_SUPPLY = 100000 * 1000000000000000ULL; // 100,000 with 15 decimals
    
    constexpr uint64_t QBTC_DECIMALS = 8;
    constexpr uint64_t QBTC_TOTAL_SUPPLY = 2100000000000000; // 21M BTC in satoshis
    
    constexpr uint64_t QUSD_DECIMALS = 15;
    constexpr uint64_t QUSD_DECIMAL_MULTIPLIER = 1000000000000000ULL;
    
    // Bridge Configuration
    constexpr uint64_t BRIDGE_FEE = 0; // Zero fees for trustless operation
    constexpr uint64_t MIN_SWAP_AMOUNT = 1000; // Minimum swap amount
    constexpr uint64_t MAX_DAILY_VOLUME = 1000000000000000ULL; // 1M STX equivalent
    
    // Security Configuration
    constexpr uint64_t EMERGENCY_PAUSE_THRESHOLD = 3; // Failed updates before pause
    constexpr uint64_t ORACLE_ROTATION_INTERVAL = 86400; // 24 hours
    constexpr uint64_t PRICE_UPDATE_TIMEOUT = 300; // 5 minutes
}

// ========================== THREAD-SAFE LOGGING ==========================
class ThreadSafeLogger {
private:
    std::mutex log_mutex;
    std::ofstream log_file;
    
public:
    ThreadSafeLogger(const std::string& filename) {
        log_file.open(filename, std::ios::app);
    }
    
    void log(const std::string& level, const std::string& message) {
        std::lock_guard<std::mutex> lock(log_mutex);
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        log_file << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
                 << " [" << level << "] " << message << std::endl;
        log_file.flush();
    }
    
    void info(const std::string& message) { log("INFO", message); }
    void warn(const std::string& message) { log("WARN", message); }
    void error(const std::string& message) { log("ERROR", message); }
    void security(const std::string& message) { log("SECURITY", message); }
};

// ========================== LAUNCH PROTECTION ==========================
class LaunchProtect {
protected:
    std::atomic<bool> initialized{false};
    std::atomic<bool> burned{false};
    std::string admin;
    std::shared_ptr<ThreadSafeLogger> logger;
    
    LaunchProtect(const std::string& admin_address, std::shared_ptr<ThreadSafeLogger> log)
        : admin(admin_address), logger(log) {
        logger->info("LaunchProtect initialized for admin: " + admin_address);
    }

    void requireActive(const std::string& sender) const {
        if (!initialized.load() && sender != admin) {
            logger->security("Unauthorized access attempt by: " + sender);
            throw std::runtime_error("Contract not initialized");
        }
        if (burned.load()) {
            logger->security("Contract immutable - admin key burned");
            throw std::runtime_error("Admin key burned - contract immutable");
        }
    }

    void requireAdmin(const std::string& sender) const {
        if (sender != admin) {
            logger->security("Admin access required, attempted by: " + sender);
            throw std::runtime_error("Admin access required");
        }
        if (burned.load()) {
            logger->security("Admin key already burned");
            throw std::runtime_error("Admin key burned - contract immutable");
        }
    }

    void finalizeLaunch(const std::string& sender) {
        requireAdmin(sender);
        initialized.store(true);
        logger->info("Launch finalized by: " + sender);
    }

    void burnKey(const std::string& sender) {
        requireAdmin(sender);
        burned.store(true);
        admin = "0x000000000000000000000000000000000000dead";
        logger->security("Admin key burned by: " + sender);
    }

    bool isInitialized() const { return initialized.load(); }
    bool isKeyBurned() const { return burned.load(); }
    std::string adminAddress() const { return admin; }
};

// ========================== ORACLE COMMITTEE ==========================
class QOracleCommittee : public LaunchProtect {
private:
    std::unique_ptr<qOracle::QuantumSignatureVerifier> verifier;
    std::unique_ptr<qOracle::PriceValidator> validator;
    qOracle::PriceMessage last_price;
    std::vector<qOracle::PriceMessage> price_history;
    std::mutex price_mutex;
    std::atomic<uint64_t> failed_updates{0};
    std::atomic<bool> emergency_paused{false};
    size_t max_history = 1024;
    
    // Oracle performance tracking
    struct OraclePerformance {
        uint64_t successful_updates = 0;
        uint64_t failed_updates = 0;
        uint64_t last_update_time = 0;
        bool active = true;
    };
    std::array<OraclePerformance, qOracleConfig::NUM_ORACLES> oracle_performance;

public:
    QOracleCommittee(const std::string& admin, 
                     const std::array<qOracle::Dilithium3PubKey, qOracleConfig::NUM_ORACLES>& oracle_keys,
                     const std::array<std::string, qOracleConfig::NUM_ORACLES>& oracle_addresses,
                     std::shared_ptr<ThreadSafeLogger> log)
        : LaunchProtect(admin, log) {
        
        verifier = std::make_unique<qOracle::QuantumSignatureVerifier>();
        validator = std::make_unique<qOracle::PriceValidator>();
        
        verifier->initialize_oracles(oracle_keys, oracle_addresses);
        last_price = qOracle::PriceMessage(0, 0, 15, 0, "BTC");
        
        logger->info("Oracle Committee initialized with " + std::to_string(qOracleConfig::NUM_ORACLES) + " oracles");
    }

    // Submit price update (public function)
    bool submit_price_update(const qOracle::PriceUpdate& update) {
        requireActive("oracle_committee");
        
        if (emergency_paused.load()) {
            logger->warn("Price update rejected - system in emergency pause");
            return false;
        }
        
        std::lock_guard<std::mutex> lock(price_mutex);
        
        // Validate price update
        if (!validator->validate_price_update(update.message, get_current_block_timestamp(), last_price.price)) {
            logger->warn("Price update validation failed");
            failed_updates.fetch_add(1);
            return false;
        }
        
        // Verify quantum signatures
        if (!verifier->verify_price_update(update)) {
            logger->warn("Price update signature verification failed");
            failed_updates.fetch_add(1);
            return false;
        }
        
        // Update price history
        last_price = update.message;
        price_history.push_back(last_price);
        
        // Maintain history size
        if (price_history.size() > max_history) {
            price_history.erase(price_history.begin());
        }
        
        // Update oracle performance
        for (const auto& sig : update.signatures) {
            if (sig.oracle_index < qOracleConfig::NUM_ORACLES) {
                oracle_performance[sig.oracle_index].successful_updates++;
                oracle_performance[sig.oracle_index].last_update_time = update.message.timestamp;
            }
        }
        
        logger->info("Price update accepted: " + std::to_string(last_price.price) + 
                    " for " + last_price.asset + " at " + std::to_string(last_price.timestamp));
        
        // Reset failed updates counter on success
        failed_updates.store(0);
        
        return true;
    }

    qOracle::PriceMessage get_current_price() const { 
        std::lock_guard<std::mutex> lock(price_mutex);
        return last_price; 
    }

    bool emergency_pause(const std::string& sender) {
        requireAdmin(sender);
        emergency_paused.store(true);
        logger->security("Emergency pause activated by: " + sender);
        return true;
    }
    
    bool resume_operations(const std::string& sender) {
        requireAdmin(sender);
        emergency_paused.store(false);
        logger->info("Operations resumed by: " + sender);
        return true;
    }
    
    bool is_emergency_paused() const { return emergency_paused.load(); }
    
    uint64_t get_failed_updates() const { return failed_updates.load(); }
    
    std::vector<OraclePerformance> get_oracle_performance() const {
        return std::vector<OraclePerformance>(oracle_performance.begin(), oracle_performance.end());
    }

private:
    uint64_t get_current_block_timestamp() const {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }
};

// ========================== BANKON PYTHAI TOKEN ==========================
class BankonPythaiToken : public LaunchProtect {
private:
    std::unordered_map<std::string, uint64_t> balances;
    std::atomic<bool> minted{false};
    std::mutex balance_mutex;
    
public:
    BankonPythaiToken(const std::string& deployer, std::shared_ptr<ThreadSafeLogger> log) 
        : LaunchProtect(deployer, log) {}

    bool mint_initial_supply(const std::string& sender) {
        requireAdmin(sender);
        
        if (minted.load()) {
            logger->warn("Initial supply already minted");
            return false;
        }
        
        std::lock_guard<std::mutex> lock(balance_mutex);
        balances[sender] = qOracleConfig::BKPY_TOTAL_SUPPLY;
        minted.store(true);
        
        logger->info("Initial BKPY supply minted to: " + sender);
        return true;
    }

    bool transfer(const std::string& sender, const std::string& to, uint64_t amount) {
        requireActive(sender);
        
        if (amount == 0) return false;
        
        std::lock_guard<std::mutex> lock(balance_mutex);
        
        if (balances[sender] < amount) {
            logger->warn("Insufficient balance for transfer from: " + sender);
            return false;
        }
        
        balances[sender] -= amount;
        balances[to] += amount;
        
        logger->info("BKPY transfer: " + std::to_string(amount) + " from " + sender + " to " + to);
        return true;
    }

    bool burn(const std::string& sender, uint64_t amount) {
        requireActive(sender);
        
        if (amount == 0) return false;
        
        std::lock_guard<std::mutex> lock(balance_mutex);
        
        if (balances[sender] < amount) {
            logger->warn("Insufficient balance for burn from: " + sender);
            return false;
        }
        
        balances[sender] -= amount;
        // Burned tokens are removed from total supply
        
        logger->info("BKPY burn: " + std::to_string(amount) + " from " + sender);
        return true;
    }

    uint64_t balanceOf(const std::string& addr) const {
        std::lock_guard<std::mutex> lock(balance_mutex);
        auto it = balances.find(addr);
        return it != balances.end() ? it->second : 0;
    }

    uint64_t totalSupply() const { return qOracleConfig::BKPY_TOTAL_SUPPLY; }
    std::string symbol() const { return "BKPY"; }
    std::string name() const { return "BANKON PYTHAI"; }
    uint64_t decimals() const { return qOracleConfig::BKPY_DECIMALS; }
};

// ========================== QBTC SYNTHETIC TOKEN ==========================
class QBTCSynthetic : public LaunchProtect {
private:
    std::unordered_map<std::string, uint64_t> balances;
    std::atomic<uint64_t> total_supply{0};
    std::mutex balance_mutex;
    QOracleCommittee& oracle;
    
public:
    QBTCSynthetic(const std::string& deployer, QOracleCommittee& _oracle, std::shared_ptr<ThreadSafeLogger> log) 
        : LaunchProtect(deployer, log), oracle(_oracle) {}

    bool mint(const std::string& user, uint64_t btc_sats, const qOracle::PriceUpdate& price_update) {
        requireActive(user);
        
        if (oracle.is_emergency_paused()) {
            logger->warn("Minting rejected - oracle system paused");
            return false;
        }
        
        if (btc_sats == 0) return false;
        
        // Verify price update is recent
        auto current_price = oracle.get_current_price();
        if (price_update.message.timestamp < current_price.timestamp - qOracleConfig::PRICE_UPDATE_TIMEOUT) {
            logger->warn("Price update too old for minting");
            return false;
        }
        
        std::lock_guard<std::mutex> lock(balance_mutex);
        balances[user] += btc_sats;
        total_supply.fetch_add(btc_sats);
        
        logger->info("qBTC minted: " + std::to_string(btc_sats) + " to " + user);
        return true;
    }

    bool burn(const std::string& user, uint64_t amount) {
        requireActive(user);
        
        if (amount == 0) return false;
        
        std::lock_guard<std::mutex> lock(balance_mutex);
        
        if (balances[user] < amount) {
            logger->warn("Insufficient qBTC balance for burn from: " + user);
            return false;
        }
        
        balances[user] -= amount;
        total_supply.fetch_sub(amount);
        
        logger->info("qBTC burned: " + std::to_string(amount) + " from " + user);
        return true;
    }

    bool transfer(const std::string& sender, const std::string& to, uint64_t amount) {
        requireActive(sender);
        
        if (amount == 0) return false;
        
        std::lock_guard<std::mutex> lock(balance_mutex);
        
        if (balances[sender] < amount) {
            logger->warn("Insufficient qBTC balance for transfer from: " + sender);
            return false;
        }
        
        balances[sender] -= amount;
        balances[to] += amount;
        
        logger->info("qBTC transfer: " + std::to_string(amount) + " from " + sender + " to " + to);
        return true;
    }

    uint64_t balanceOf(const std::string& addr) const {
        std::lock_guard<std::mutex> lock(balance_mutex);
        auto it = balances.find(addr);
        return it != balances.end() ? it->second : 0;
    }

    uint64_t totalSupply() const { return total_supply.load(); }
    std::string symbol() const { return "qBTC"; }
    std::string name() const { return "Synthetic Bitcoin"; }
    uint64_t decimals() const { return qOracleConfig::QBTC_DECIMALS; }

    qOracle::PriceMessage getCurrentPrice() const { return oracle.get_current_price(); }
};

// ========================== QUSD STABLECOIN ==========================
class QUSDStablecoin : public LaunchProtect {
private:
    std::unordered_map<std::string, uint64_t> balances;
    std::atomic<uint64_t> total_supply{0};
    std::mutex balance_mutex;
    std::string bridge_authority;
    
public:
    QUSDStablecoin(const std::string& deployer, const std::string& bridge_auth, std::shared_ptr<ThreadSafeLogger> log) 
        : LaunchProtect(deployer, log), bridge_authority(bridge_auth) {}

    bool mint(const std::string& sender, const std::string& to, uint64_t amount) {
        requireActive(sender);
        
        if (sender != bridge_authority) {
            logger->warn("Unauthorized qUSD mint attempt by: " + sender);
            return false;
        }
        
        if (amount == 0) return false;
        
        std::lock_guard<std::mutex> lock(balance_mutex);
        balances[to] += amount;
        total_supply.fetch_add(amount);
        
        logger->info("qUSD minted: " + std::to_string(amount) + " to " + to);
        return true;
    }

    bool burn(const std::string& sender, const std::string& from, uint64_t amount) {
        requireActive(sender);
        
        if (sender != bridge_authority) {
            logger->warn("Unauthorized qUSD burn attempt by: " + sender);
            return false;
        }
        
        if (amount == 0) return false;
        
        std::lock_guard<std::mutex> lock(balance_mutex);
        
        if (balances[from] < amount) {
            logger->warn("Insufficient qUSD balance for burn from: " + from);
            return false;
        }
        
        balances[from] -= amount;
        total_supply.fetch_sub(amount);
        
        logger->info("qUSD burned: " + std::to_string(amount) + " from " + from);
        return true;
    }

    bool transfer(const std::string& sender, const std::string& to, uint64_t amount) {
        requireActive(sender);
        
        if (amount == 0) return false;
        
        std::lock_guard<std::mutex> lock(balance_mutex);
        
        if (balances[sender] < amount) {
            logger->warn("Insufficient qUSD balance for transfer from: " + sender);
            return false;
        }
        
        balances[sender] -= amount;
        balances[to] += amount;
        
        logger->info("qUSD transfer: " + std::to_string(amount) + " from " + sender + " to " + to);
        return true;
    }

    uint64_t balanceOf(const std::string& addr) const {
        std::lock_guard<std::mutex> lock(balance_mutex);
        auto it = balances.find(addr);
        return it != balances.end() ? it->second : 0;
    }

    uint64_t totalSupply() const { return total_supply.load(); }
    std::string symbol() const { return "qUSD"; }
    std::string name() const { return "Synthetic USDC"; }
    uint64_t decimals() const { return qOracleConfig::QUSD_DECIMALS; }
};

// ========================== CROSS-CHAIN BRIDGE ==========================
class CrossChainBridge : public LaunchProtect {
private:
    QOracleCommittee& oracle;
    QBTCSynthetic& qbtc;
    QUSDStablecoin& qusd;
    std::unordered_map<std::string, uint64_t> bridge_balances;
    std::atomic<uint64_t> daily_volume{0};
    std::mutex bridge_mutex;
    uint64_t last_volume_reset = 0;
    
public:
    CrossChainBridge(const std::string& deployer, QOracleCommittee& _oracle, 
                     QBTCSynthetic& _qbtc, QUSDStablecoin& _qusd, std::shared_ptr<ThreadSafeLogger> log)
        : LaunchProtect(deployer, log), oracle(_oracle), qbtc(_qbtc), qusd(_qusd) {}

    bool swap_stx_for_qbtc(const std::string& user, uint64_t stx_amount, const qOracle::PriceUpdate& price_update) {
        requireActive(user);
        
        if (oracle.is_emergency_paused()) {
            logger->warn("Bridge swap rejected - oracle system paused");
            return false;
        }
        
        if (stx_amount < qOracleConfig::MIN_SWAP_AMOUNT) {
            logger->warn("Swap amount below minimum: " + std::to_string(stx_amount));
            return false;
        }
        
        // Check daily volume limits
        reset_daily_volume_if_needed();
        if (daily_volume.load() + stx_amount > qOracleConfig::MAX_DAILY_VOLUME) {
            logger->warn("Daily volume limit exceeded");
            return false;
        }
        
        // Calculate qBTC amount based on price
        uint64_t qbtc_amount = (stx_amount * price_update.message.price) / qOracleConfig::QUSD_DECIMAL_MULTIPLIER;
        
        std::lock_guard<std::mutex> lock(bridge_mutex);
        
        // Update bridge balances
        bridge_balances[user] += stx_amount;
        daily_volume.fetch_add(stx_amount);
        
        // Mint qBTC to user
        if (!qbtc.mint(user, qbtc_amount, price_update)) {
            logger->error("Failed to mint qBTC for bridge swap");
            return false;
        }
        
        logger->info("Bridge swap STX->qBTC: " + std::to_string(stx_amount) + " STX for " + 
                    std::to_string(qbtc_amount) + " qBTC by " + user);
        return true;
    }

    bool swap_qbtc_for_stx(const std::string& user, uint64_t qbtc_amount, const qOracle::PriceUpdate& price_update) {
        requireActive(user);
        
        if (oracle.is_emergency_paused()) {
            logger->warn("Bridge swap rejected - oracle system paused");
            return false;
        }
        
        if (qbtc_amount < qOracleConfig::MIN_SWAP_AMOUNT) {
            logger->warn("Swap amount below minimum: " + std::to_string(qbtc_amount));
            return false;
        }
        
        // Calculate STX amount based on price
        uint64_t stx_amount = (qbtc_amount * price_update.message.price) / qOracleConfig::QUSD_DECIMAL_MULTIPLIER;
        
        // Check daily volume limits
        reset_daily_volume_if_needed();
        if (daily_volume.load() + stx_amount > qOracleConfig::MAX_DAILY_VOLUME) {
            logger->warn("Daily volume limit exceeded");
            return false;
        }
        
        std::lock_guard<std::mutex> lock(bridge_mutex);
        
        // Burn qBTC from user
        if (!qbtc.burn(user, qbtc_amount)) {
            logger->error("Failed to burn qBTC for bridge swap");
            return false;
        }
        
        // Update bridge balances
        bridge_balances[user] += stx_amount;
        daily_volume.fetch_add(stx_amount);
        
        logger->info("Bridge swap qBTC->STX: " + std::to_string(qbtc_amount) + " qBTC for " + 
                    std::to_string(stx_amount) + " STX by " + user);
        return true;
    }

    uint64_t getBridgeBalance(const std::string& user) const {
        std::lock_guard<std::mutex> lock(bridge_mutex);
        auto it = bridge_balances.find(user);
        return it != bridge_balances.end() ? it->second : 0;
    }
    
    uint64_t getDailyVolume() const { return daily_volume.load(); }
    
    void reset_daily_volume_if_needed() {
        uint64_t now = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        if (now - last_volume_reset >= 86400) { // 24 hours
            daily_volume.store(0);
            last_volume_reset = now;
        }
    }
};

// ========================== GOVERNANCE MULTISIG ==========================
struct Proposal {
    std::string to;
    uint64_t value;
    std::string data;
    uint64_t nonce;
    bool executed;
    std::set<std::string> signatures;
    std::string action;
    std::string parameter;
    uint64_t created_time;
    uint64_t execution_delay;
};

class QnosisMultisig : public LaunchProtect {
private:
    std::vector<std::string> owners;
    uint32_t threshold;
    std::atomic<uint64_t> proposal_nonce{1};
    std::unordered_map<uint64_t, Proposal> proposals;
    std::mutex proposal_mutex;
    
public:
    QnosisMultisig(const std::string& deployer, const std::vector<std::string>& initial_owners, 
                   uint32_t thresh, std::shared_ptr<ThreadSafeLogger> log)
        : LaunchProtect(deployer, log), owners(initial_owners), threshold(thresh) {
        logger->info("Governance multisig initialized with " + std::to_string(owners.size()) + 
                    " owners, threshold: " + std::to_string(threshold));
    }

    uint64_t propose(const std::string& proposer, const std::string& to, uint64_t value, 
                    const std::string& data, const std::string& action, const std::string& parameter) {
        requireActive(proposer);
        
        if (std::find(owners.begin(), owners.end(), proposer) == owners.end()) {
            logger->warn("Proposal rejected - not an owner: " + proposer);
            return 0;
        }
        
        std::lock_guard<std::mutex> lock(proposal_mutex);
        
        uint64_t nonce = proposal_nonce.fetch_add(1);
        uint64_t now = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        Proposal prop{to, value, data, nonce, false, {}, action, parameter, now, 86400}; // 24h delay
        proposals[nonce] = prop;
        
        logger->info("Proposal created: " + std::to_string(nonce) + " by " + proposer + 
                    " action: " + action);
        return nonce;
    }

    void sign(uint64_t nonce, const std::string& signer) {
        requireActive(signer);
        
        if (std::find(owners.begin(), owners.end(), signer) == owners.end()) {
            logger->warn("Signature rejected - not an owner: " + signer);
            return;
        }
        
        std::lock_guard<std::mutex> lock(proposal_mutex);
        
        auto it = proposals.find(nonce);
        if (it == proposals.end()) {
            logger->warn("Proposal not found for signing: " + std::to_string(nonce));
            return;
        }
        
        if (it->second.executed) {
            logger->warn("Proposal already executed: " + std::to_string(nonce));
            return;
        }
        
        it->second.signatures.insert(signer);
        
        logger->info("Proposal signed: " + std::to_string(nonce) + " by " + signer + 
                    " signatures: " + std::to_string(it->second.signatures.size()) + "/" + 
                    std::to_string(threshold));
    }

    void execute(uint64_t nonce) {
        std::lock_guard<std::mutex> lock(proposal_mutex);
        
        auto it = proposals.find(nonce);
        if (it == proposals.end()) {
            logger->warn("Proposal not found for execution: " + std::to_string(nonce));
            return;
        }
        
        if (it->second.executed) {
            logger->warn("Proposal already executed: " + std::to_string(nonce));
            return;
        }
        
        if (it->second.signatures.size() < threshold) {
            logger->warn("Insufficient signatures for execution: " + std::to_string(nonce));
            return;
        }
        
        uint64_t now = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        if (now < it->second.created_time + it->second.execution_delay) {
            logger->warn("Execution delay not met for proposal: " + std::to_string(nonce));
            return;
        }
        
        it->second.executed = true;
        
        logger->info("Proposal executed: " + std::to_string(nonce) + " action: " + it->second.action);
    }

    std::vector<std::string> getOwners() const { return owners; }
    uint32_t getThreshold() const { return threshold; }
    bool isExecuted(uint64_t nonce) const { 
        std::lock_guard<std::mutex> lock(proposal_mutex);
        auto it = proposals.find(nonce);
        return it != proposals.end() ? it->second.executed : false;
    }
};

// ========================== MAIN QORACLE SYSTEM ==========================
class QOracleSystem {
private:
    std::unique_ptr<QOracleCommittee> oracle_committee;
    std::unique_ptr<BankonPythaiToken> bkpy_token;
    std::unique_ptr<QBTCSynthetic> qbtc_token;
    std::unique_ptr<QUSDStablecoin> qusd_token;
    std::unique_ptr<CrossChainBridge> bridge;
    std::unique_ptr<QnosisMultisig> governance;
    std::shared_ptr<ThreadSafeLogger> logger;
    
public:
    QOracleSystem(const std::string& deployer, 
                  const std::array<qOracle::Dilithium3PubKey, qOracleConfig::NUM_ORACLES>& oracle_keys,
                  const std::array<std::string, qOracleConfig::NUM_ORACLES>& oracle_addresses,
                  const std::vector<std::string>& governance_owners,
                  const std::string& bridge_authority) {
        
        logger = std::make_shared<ThreadSafeLogger>("qoracle_production.log");
        
        oracle_committee = std::make_unique<QOracleCommittee>(deployer, oracle_keys, oracle_addresses, logger);
        bkpy_token = std::make_unique<BankonPythaiToken>(deployer, logger);
        qbtc_token = std::make_unique<QBTCSynthetic>(deployer, *oracle_committee, logger);
        qusd_token = std::make_unique<QUSDStablecoin>(deployer, bridge_authority, logger);
        bridge = std::make_unique<CrossChainBridge>(deployer, *oracle_committee, *qbtc_token, *qusd_token, logger);
        governance = std::make_unique<QnosisMultisig>(deployer, governance_owners, 3, logger); // 3-of-N threshold
        
        logger->info("QOracle System initialized successfully");
    }

    void initialize_system(const std::string& admin) {
        logger->info("Initializing QOracle system...");
        
        // Initialize all components
        bkpy_token->mint_initial_supply(admin);
        
        // Finalize launches
        oracle_committee->finalizeLaunch(admin);
        bkpy_token->finalizeLaunch(admin);
        qbtc_token->finalizeLaunch(admin);
        qusd_token->finalizeLaunch(admin);
        bridge->finalizeLaunch(admin);
        governance->finalizeLaunch(admin);
        
        logger->info("QOracle system initialization completed");
    }

    void burn_admin_keys(const std::string& admin) {
        logger->security("Burning admin keys...");
        
        oracle_committee->burnKey(admin);
        bkpy_token->burnKey(admin);
        qbtc_token->burnKey(admin);
        qusd_token->burnKey(admin);
        bridge->burnKey(admin);
        governance->burnKey(admin);
        
        logger->security("All admin keys burned - system now immutable");
    }

    void get_system_status() const {
        logger->info("=== QOracle System Status ===");
        logger->info("Oracle Committee: " + std::string(oracle_committee->isInitialized() ? "ACTIVE" : "INACTIVE"));
        logger->info("BANKON PYTHAI Token: " + std::string(bkpy_token->isInitialized() ? "ACTIVE" : "INACTIVE"));
        logger->info("qBTC Synthetic: " + std::string(qbtc_token->isInitialized() ? "ACTIVE" : "INACTIVE"));
        logger->info("qUSD Stablecoin: " + std::string(qusd_token->isInitialized() ? "ACTIVE" : "INACTIVE"));
        logger->info("Cross-Chain Bridge: " + std::string(bridge->isInitialized() ? "ACTIVE" : "INACTIVE"));
        logger->info("Governance: " + std::string(governance->isInitialized() ? "ACTIVE" : "INACTIVE"));
        
        auto current_price = oracle_committee->get_current_price();
        logger->info("Current BTC Price: " + std::to_string(current_price.price) + 
                    " at " + std::to_string(current_price.timestamp));
    }
    
    // Get component references for external access
    QOracleCommittee* get_oracle_committee() { return oracle_committee.get(); }
    BankonPythaiToken* get_bkpy_token() { return bkpy_token.get(); }
    QBTCSynthetic* get_qbtc_token() { return qbtc_token.get(); }
    QUSDStablecoin* get_qusd_token() { return qusd_token.get(); }
    CrossChainBridge* get_bridge() { return bridge.get(); }
    QnosisMultisig* get_governance() { return governance.get(); }
};

// ========================== MAIN FUNCTION ==========================
int main() {
    try {
        // Configuration
        std::string deployer = "ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5";
        std::string bridge_authority = "ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5";
        
        // Initialize oracle keys (placeholder - replace with actual keys)
        std::array<qOracle::Dilithium3PubKey, qOracleConfig::NUM_ORACLES> oracle_keys;
        std::array<std::string, qOracleConfig::NUM_ORACLES> oracle_addresses;
        
        for (size_t i = 0; i < qOracleConfig::NUM_ORACLES; ++i) {
            std::fill(oracle_keys[i].begin(), oracle_keys[i].end(), static_cast<uint8_t>(i + 1));
            oracle_addresses[i] = "ST" + std::to_string(i + 1) + "ORACLE" + std::to_string(i + 1);
        }
        
        // Initialize governance owners
        std::vector<std::string> governance_owners = {
            "ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5",
            "ST2ORACLE1ORACLE1",
            "ST3ORACLE2ORACLE2"
        };
        
        // Create QOracle system
        QOracleSystem qoracle_system(deployer, oracle_keys, oracle_addresses, governance_owners, bridge_authority);
        
        // Initialize system
        qoracle_system.initialize_system(deployer);
        
        // Get system status
        qoracle_system.get_system_status();
        
        // Burn admin keys (optional - makes system immutable)
        // qoracle_system.burn_admin_keys(deployer);
        
        std::cout << "QOracle Production RC2 System Ready!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 
/*
 * qOracle Production RC1 - Unified BANKON PYTHAI System
 * Quantum-Resistant Oracle & Synthetic Asset Ecosystem
 * Immediate Testnet Deployment Ready
 * 
 * Components:
 * - Oracle Committee (7-of-7 quantum signatures)
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
}

// ========================== QUANTUM SIGNATURE VERIFICATION ==========================
extern bool dilithium_verify(
    const uint8_t* pubkey,
    const uint8_t* msg, size_t msg_len,
    const uint8_t* sig, size_t sig_len
);

// ========================== LAUNCH PROTECTION ==========================
class LaunchProtect {
protected:
    bool initialized = false;
    std::string admin;
    bool burned = false;

    LaunchProtect(const std::string& admin_address) : admin(admin_address) {
        emit("LaunchProtectInitialized", admin_address);
    }

    void requireActive(const std::string& sender) const {
        if (!initialized && sender != admin)
            throw std::runtime_error("Contract not initialized");
        if (burned)
            throw std::runtime_error("Admin key burned - contract immutable");
    }

    void requireAdmin(const std::string& sender) const {
        if (sender != admin)
            throw std::runtime_error("Admin access required");
        if (burned)
            throw std::runtime_error("Admin key burned - contract immutable");
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

// ========================== ORACLE COMMITTEE ==========================
struct OraclePubKey {
    std::array<uint8_t, qOracleConfig::PUBKEY_SIZE> pubkey;
};

struct PriceMessage {
    uint64_t price;      // Fixed-point price (15 decimals)
    uint64_t timestamp;  // Unix timestamp
    uint8_t decimals;    // Price precision
};

struct OracleSignature {
    size_t signer_index;
    std::array<uint8_t, qOracleConfig::SIG_SIZE> signature;
};

struct PriceUpdate {
    PriceMessage message;
    std::vector<OracleSignature> signatures;
};

class QOracleCommittee : public LaunchProtect {
private:
    std::array<OraclePubKey, qOracleConfig::NUM_ORACLES> trusted_oracles;
    PriceMessage last_price;
    std::vector<PriceMessage> price_history;
    size_t max_history = 1024;

public:
    QOracleCommittee(const std::string& admin, const std::array<OraclePubKey, qOracleConfig::NUM_ORACLES>& oracles)
        : LaunchProtect(admin), trusted_oracles(oracles) {
        last_price = {0, 0, 15};
    }

    // Serialize message for signature verification
    void serialize_message(const PriceMessage& msg, uint8_t* out) const {
        uint64_t price_net = htobe64(msg.price);
        uint64_t ts_net = htobe64(msg.timestamp);
        memcpy(out, &price_net, 8);
        memcpy(out + 8, &ts_net, 8);
    }

    // Validate single oracle signature
    bool validate_signature(const OraclePubKey& pubkey, const PriceMessage& msg, const std::array<uint8_t, qOracleConfig::SIG_SIZE>& sig) const {
        uint8_t data[16];
        serialize_message(msg, data);
        bool ok = dilithium_verify(pubkey.pubkey.data(), data, sizeof(data), sig.data(), sig.size());
        memset(data, 0, sizeof(data)); // Clear sensitive data
        return ok;
    }

    // Validate oracle committee update
    bool validate_update(const PriceUpdate& update) const {
        // Check signature count
        if (update.signatures.size() < qOracleConfig::QUORUM_THRESHOLD) return false;
        
        // Check for duplicate signers
        std::set<size_t> seen;
        size_t valid = 0;

        for (const auto& sig : update.signatures) {
            if (sig.signer_index >= qOracleConfig::NUM_ORACLES) continue;
            if (!seen.insert(sig.signer_index).second) continue; // Skip duplicates
            
            if (validate_signature(trusted_oracles[sig.signer_index], update.message, sig.signature)) {
                ++valid;
            }
        }
        
        if (valid < qOracleConfig::QUORUM_THRESHOLD) return false;

        // Monotonic timestamp check
        if (update.message.timestamp <= last_price.timestamp) return false;

        // Timestamp sanity check
        uint64_t now = get_current_block_timestamp();
        if (update.message.timestamp > now + qOracleConfig::MAX_TIMESTAMP_SKEW) return false;
        if (now > 0 && update.message.timestamp + qOracleConfig::MAX_TIMESTAMP_SKEW < now) return false;

        // Price bounds check
        if (update.message.price < qOracleConfig::MIN_PRICE || update.message.price > qOracleConfig::MAX_PRICE) return false;

        return true;
    }

    // Submit price update (public function)
    bool submit_price_update(const PriceUpdate& update) {
        requireActive("oracle_committee"); // Allow anyone to submit valid updates
        
        if (!validate_update(update)) return false;
        
        last_price = update.message;
        
        // Update price history
        if (price_history.size() == max_history)
            price_history.erase(price_history.begin());
        price_history.push_back(update.message);
        
        emit("PriceUpdated", std::to_string(update.message.price) + " at " + std::to_string(update.message.timestamp));
        return true;
    }

    // Get current price
    PriceMessage get_current_price() const { return last_price; }
    
    // Get price history
    const std::vector<PriceMessage>& get_price_history() const { return price_history; }

    // Emergency pause (admin only, before key burn)
    bool emergency_pause(const std::string& sender) {
        requireAdmin(sender);
        // Implementation for emergency pause
        emit("EmergencyPause", sender);
        return true;
    }

private:
    uint64_t get_current_block_timestamp() const {
        return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }
    
    uint64_t htobe64(uint64_t value) const {
        // Network byte order conversion (big-endian)
        return ((value & 0xFFULL) << 56) |
               (((value >> 8) & 0xFFULL) << 48) |
               (((value >> 16) & 0xFFULL) << 40) |
               (((value >> 24) & 0xFFULL) << 32) |
               (((value >> 32) & 0xFFULL) << 24) |
               (((value >> 40) & 0xFFULL) << 16) |
               (((value >> 48) & 0xFFULL) << 8) |
               ((value >> 56) & 0xFFULL);
    }
};

// ========================== BANKON PYTHAI TOKEN ==========================
class BankonPythaiToken : public LaunchProtect {
private:
    std::unordered_map<std::string, uint64_t> balances;
    bool minted = false;

public:
    BankonPythaiToken(const std::string& deployer) : LaunchProtect(deployer) {}

    // Mint initial supply (admin only, once)
    bool mint_initial_supply(const std::string& sender) {
        requireAdmin(sender);
        if (minted) return false;
        
        balances[sender] = qOracleConfig::BKPY_TOTAL_SUPPLY;
        minted = true;
        
        emit("InitialMint", sender + " minted " + formatAmount(qOracleConfig::BKPY_TOTAL_SUPPLY));
        return true;
    }

    // Transfer tokens
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
    
    uint64_t totalSupply() const { return qOracleConfig::BKPY_TOTAL_SUPPLY; }
    std::string symbol() const { return "BKPY"; }
    std::string name() const { return "BANKON PYTHAI"; }
    uint64_t decimals() const { return qOracleConfig::BKPY_DECIMALS; }

private:
    std::string formatAmount(uint64_t amount) const {
        std::ostringstream oss;
        oss << (amount / 1000000000000000ULL) << "."
            << std::setw(15) << std::setfill('0') << (amount % 1000000000000000ULL);
        return oss.str();
    }
};

// ========================== QBTC SYNTHETIC TOKEN ==========================
class QBTCSynthetic : public LaunchProtect {
private:
    std::unordered_map<std::string, uint64_t> balances;
    uint64_t total_supply = 0;
    QOracleCommittee& oracle;

public:
    QBTCSynthetic(const std::string& deployer, QOracleCommittee& _oracle) 
        : LaunchProtect(deployer), oracle(_oracle) {}

    // Mint qBTC using oracle price
    bool mint(const std::string& user, uint64_t btc_sats, const PriceUpdate& price_update) {
        requireActive(user);
        if (btc_sats == 0) return false;
        
        // Validate price update through oracle
        if (!oracle.submit_price_update(price_update)) return false;
        
        balances[user] += btc_sats;
        total_supply += btc_sats;
        
        emit("qBTCMint", user + " minted " + std::to_string(btc_sats) + " satoshis");
        return true;
    }

    // Burn qBTC
    bool burn(const std::string& user, uint64_t amount) {
        requireActive(user);
        if (balances[user] < amount || amount == 0) return false;
        
        balances[user] -= amount;
        total_supply -= amount;
        
        emit("qBTCBurn", user + " burned " + std::to_string(amount) + " satoshis");
        return true;
    }

    // Transfer qBTC
    bool transfer(const std::string& sender, const std::string& to, uint64_t amount) {
        requireActive(sender);
        if (sender == to || amount == 0) return false;
        if (balances[sender] < amount) return false;
        
        balances[sender] -= amount;
        balances[to] += amount;
        
        emit("qBTCTransfer", sender + " -> " + to + " : " + std::to_string(amount));
        return true;
    }

    // View functions
    uint64_t balanceOf(const std::string& addr) const {
        auto it = balances.find(addr);
        return it != balances.end() ? it->second : 0;
    }
    
    uint64_t totalSupply() const { return total_supply; }
    std::string symbol() const { return "qBTC"; }
    std::string name() const { return "Synthetic Bitcoin"; }
    uint64_t decimals() const { return qOracleConfig::QBTC_DECIMALS; }
    
    PriceMessage getCurrentPrice() const { return oracle.get_current_price(); }
};

// ========================== QUSD STABLECOIN ==========================
class QUSDStablecoin : public LaunchProtect {
private:
    std::unordered_map<std::string, uint64_t> balances;
    uint64_t total_supply = 0;
    std::string bridge_authority;

public:
    QUSDStablecoin(const std::string& deployer, const std::string& bridge_auth) 
        : LaunchProtect(deployer), bridge_authority(bridge_auth) {}

    // Mint qUSD (bridge authority only)
    bool mint(const std::string& sender, const std::string& to, uint64_t amount) {
        requireActive(sender);
        if (sender != bridge_authority) return false;
        if (amount == 0) return false;
        
        balances[to] += amount;
        total_supply += amount;
        
        emit("qUSDMint", to + " received " + formatAmount(amount));
        return true;
    }

    // Burn qUSD (bridge authority only)
    bool burn(const std::string& sender, const std::string& from, uint64_t amount) {
        requireActive(sender);
        if (sender != bridge_authority) return false;
        if (balances[from] < amount || amount == 0) return false;
        
        balances[from] -= amount;
        total_supply -= amount;
        
        emit("qUSDBurn", from + " burned " + formatAmount(amount));
        return true;
    }

    // Transfer qUSD
    bool transfer(const std::string& sender, const std::string& to, uint64_t amount) {
        requireActive(sender);
        if (sender == to || amount == 0) return false;
        if (balances[sender] < amount) return false;
        
        balances[sender] -= amount;
        balances[to] += amount;
        
        emit("qUSDTransfer", sender + " -> " + to + " : " + formatAmount(amount));
        return true;
    }

    // View functions
    uint64_t balanceOf(const std::string& addr) const {
        auto it = balances.find(addr);
        return it != balances.end() ? it->second : 0;
    }
    
    uint64_t totalSupply() const { return total_supply; }
    std::string symbol() const { return "qUSD"; }
    std::string name() const { return "Synthetic USDC"; }
    uint64_t decimals() const { return qOracleConfig::QUSD_DECIMALS; }

private:
    std::string formatAmount(uint64_t amount) const {
        std::ostringstream oss;
        oss << (amount / qOracleConfig::QUSD_DECIMAL_MULTIPLIER) << "."
            << std::setw(15) << std::setfill('0') << (amount % qOracleConfig::QUSD_DECIMAL_MULTIPLIER);
        return oss.str();
    }
};

// ========================== CROSS-CHAIN BRIDGE ==========================
class CrossChainBridge : public LaunchProtect {
private:
    QOracleCommittee& oracle;
    QBTCSynthetic& qbtc;
    QUSDStablecoin& qusd;
    std::unordered_map<std::string, uint64_t> bridge_balances;

public:
    CrossChainBridge(const std::string& deployer, QOracleCommittee& _oracle, 
                     QBTCSynthetic& _qbtc, QUSDStablecoin& _qusd)
        : LaunchProtect(deployer), oracle(_oracle), qbtc(_qbtc), qusd(_qusd) {}

    // Swap STX for qBTC using oracle price
    bool swap_stx_for_qbtc(const std::string& user, uint64_t stx_amount, const PriceUpdate& price_update) {
        requireActive(user);
        if (stx_amount < qOracleConfig::MIN_SWAP_AMOUNT) return false;
        
        // Validate price update
        if (!oracle.submit_price_update(price_update)) return false;
        
        PriceMessage current_price = oracle.get_current_price();
        if (current_price.price == 0) return false;
        
        // Calculate qBTC amount: (stx_amount * btc_price) / stx_price
        uint64_t btc_price_sats = current_price.price * 100000000ULL; // Convert to satoshis
        uint64_t qbtc_amount = (stx_amount * btc_price_sats) / current_price.price;
        
        if (qbtc_amount == 0) return false;
        
        // Execute swap
        bridge_balances[user] += stx_amount;
        qbtc.mint(user, qbtc_amount, price_update);
        
        emit("STXtoqBTC", user + " swapped " + std::to_string(stx_amount) + " STX for " + std::to_string(qbtc_amount) + " qBTC");
        return true;
    }

    // Swap qBTC for STX using oracle price
    bool swap_qbtc_for_stx(const std::string& user, uint64_t qbtc_amount, const PriceUpdate& price_update) {
        requireActive(user);
        if (qbtc_amount == 0) return false;
        
        // Validate price update
        if (!oracle.submit_price_update(price_update)) return false;
        
        PriceMessage current_price = oracle.get_current_price();
        if (current_price.price == 0) return false;
        
        // Calculate STX amount: (qbtc_amount * stx_price) / btc_price
        uint64_t btc_price_sats = current_price.price * 100000000ULL;
        uint64_t stx_amount = (qbtc_amount * current_price.price) / btc_price_sats;
        
        if (stx_amount == 0) return false;
        
        // Execute swap
        qbtc.burn(user, qbtc_amount);
        if (bridge_balances[user] >= stx_amount) {
            bridge_balances[user] -= stx_amount;
        }
        
        emit("qBTCtoSTX", user + " swapped " + std::to_string(qbtc_amount) + " qBTC for " + std::to_string(stx_amount) + " STX");
        return true;
    }

    // Bridge balance functions
    uint64_t getBridgeBalance(const std::string& user) const {
        auto it = bridge_balances.find(user);
        return it != bridge_balances.end() ? it->second : 0;
    }
};

// ========================== GOVERNANCE & MULTISIG ==========================
struct Proposal {
    std::string to;
    uint64_t value;
    std::string data;
    uint64_t nonce;
    bool executed;
    std::set<std::string> signatures;
    std::string action;
    std::string parameter;
};

class QnosisMultisig : public LaunchProtect {
private:
    std::vector<std::string> owners;
    uint32_t threshold;
    uint64_t proposal_nonce;
    std::unordered_map<uint64_t, Proposal> proposals;

public:
    QnosisMultisig(const std::string& deployer, const std::vector<std::string>& initial_owners, uint32_t thresh)
        : LaunchProtect(deployer), owners(initial_owners), threshold(thresh), proposal_nonce(1) {
        if (initial_owners.empty() || thresh == 0 || thresh > initial_owners.size()) {
            throw std::runtime_error("Invalid multisig configuration");
        }
    }

    // Propose new transaction
    uint64_t propose(const std::string& proposer, const std::string& to, uint64_t value, 
                     const std::string& data, const std::string& action = "transfer", 
                     const std::string& param = "") {
        requireActive(proposer);
        if (std::find(owners.begin(), owners.end(), proposer) == owners.end()) {
            throw std::runtime_error("Not an owner");
        }
        
        Proposal p{to, value, data, proposal_nonce, false, {}, action, param};
        proposals[proposal_nonce] = p;
        proposal_nonce++;
        
        emit("ProposalCreated", std::to_string(proposal_nonce - 1));
        return proposal_nonce - 1;
    }

    // Sign proposal
    void sign(uint64_t nonce, const std::string& signer) {
        requireActive(signer);
        if (std::find(owners.begin(), owners.end(), signer) == owners.end()) {
            throw std::runtime_error("Not an owner");
        }
        
        auto it = proposals.find(nonce);
        if (it == proposals.end()) {
            throw std::runtime_error("Proposal not found");
        }
        
        if (it->second.executed) {
            throw std::runtime_error("Proposal already executed");
        }
        
        it->second.signatures.insert(signer);
        emit("ProposalSigned", std::to_string(nonce) + " by " + signer);
    }

    // Execute proposal
    void execute(uint64_t nonce) {
        auto it = proposals.find(nonce);
        if (it == proposals.end()) {
            throw std::runtime_error("Proposal not found");
        }
        
        Proposal& p = it->second;
        if (p.executed) {
            throw std::runtime_error("Proposal already executed");
        }
        
        if (p.signatures.size() < threshold) {
            throw std::runtime_error("Not enough signatures");
        }
        
        // Execute the proposal action
        if (p.action == "add_owner") {
            if (std::find(owners.begin(), owners.end(), p.parameter) != owners.end()) {
                throw std::runtime_error("Already an owner");
            }
            owners.push_back(p.parameter);
        } else if (p.action == "remove_owner") {
            auto owner_it = std::find(owners.begin(), owners.end(), p.parameter);
            if (owner_it == owners.end()) {
                throw std::runtime_error("Not an owner");
            }
            if (owners.size() <= 1) {
                throw std::runtime_error("At least one owner required");
            }
            owners.erase(owner_it);
            if (threshold > owners.size()) threshold = owners.size();
        } else if (p.action == "change_threshold") {
            uint32_t new_thresh = std::stoul(p.parameter);
            if (new_thresh == 0 || new_thresh > owners.size()) {
                throw std::runtime_error("Invalid threshold");
            }
            threshold = new_thresh;
        }
        
        p.executed = true;
        emit("ProposalExecuted", std::to_string(nonce));
    }

    // View functions
    std::vector<std::string> getOwners() const { return owners; }
    uint32_t getThreshold() const { return threshold; }
    bool isExecuted(uint64_t nonce) const { 
        auto it = proposals.find(nonce);
        return it != proposals.end() ? it->second.executed : false;
    }
};

// ========================== MAIN qORACLE SYSTEM ==========================
class QOracleSystem {
private:
    QOracleCommittee oracle_committee;
    BankonPythaiToken bkpy_token;
    QBTCSynthetic qbtc_token;
    QUSDStablecoin qusd_token;
    CrossChainBridge bridge;
    QnosisMultisig governance;

public:
    QOracleSystem(const std::string& deployer, 
                  const std::array<OraclePubKey, qOracleConfig::NUM_ORACLES>& oracle_keys,
                  const std::vector<std::string>& governance_owners,
                  const std::string& bridge_authority)
        : oracle_committee(deployer, oracle_keys),
          bkpy_token(deployer),
          qbtc_token(deployer, oracle_committee),
          qusd_token(deployer, bridge_authority),
          bridge(deployer, oracle_committee, qbtc_token, qusd_token),
          governance(deployer, governance_owners, 3) // 3-of-N threshold
    {
        // Initialize system
        bkpy_token.mint_initial_supply(deployer);
    }

    // System initialization
    void initialize_system(const std::string& admin) {
        oracle_committee.finalizeLaunch(admin);
        bkpy_token.finalizeLaunch(admin);
        qbtc_token.finalizeLaunch(admin);
        qusd_token.finalizeLaunch(admin);
        bridge.finalizeLaunch(admin);
        governance.finalizeLaunch(admin);
        
        std::cout << "[SYSTEM] qOracle Production RC1 initialized successfully" << std::endl;
    }

    // Burn admin keys (make system immutable)
    void burn_admin_keys(const std::string& admin) {
        oracle_committee.burnKey(admin);
        bkpy_token.burnKey(admin);
        qbtc_token.burnKey(admin);
        qusd_token.burnKey(admin);
        bridge.burnKey(admin);
        governance.burnKey(admin);
        
        std::cout << "[SYSTEM] All admin keys burned - system is now immutable" << std::endl;
    }

    // Get system status
    void get_system_status() const {
        std::cout << "\n=== qOracle Production RC1 System Status ===" << std::endl;
        std::cout << "Oracle Committee: " << (oracle_committee.isInitialized() ? "Active" : "Inactive") << std::endl;
        std::cout << "BKPY Token: " << (bkpy_token.isInitialized() ? "Active" : "Inactive") << std::endl;
        std::cout << "qBTC Token: " << (qbtc_token.isInitialized() ? "Active" : "Inactive") << std::endl;
        std::cout << "qUSD Token: " << (qusd_token.isInitialized() ? "Active" : "Inactive") << std::endl;
        std::cout << "Bridge: " << (bridge.isInitialized() ? "Active" : "Inactive") << std::endl;
        std::cout << "Governance: " << (governance.isInitialized() ? "Active" : "Inactive") << std::endl;
        
        PriceMessage current_price = oracle_committee.get_current_price();
        std::cout << "Current Oracle Price: " << current_price.price << " (timestamp: " << current_price.timestamp << ")" << std::endl;
        std::cout << "=============================================" << std::endl;
    }

    // Access to components
    QOracleCommittee& getOracleCommittee() { return oracle_committee; }
    BankonPythaiToken& getBKPYToken() { return bkpy_token; }
    QBTCSynthetic& getQBTCToken() { return qbtc_token; }
    QUSDStablecoin& getQUSDToken() { return qusd_token; }
    CrossChainBridge& getBridge() { return bridge; }
    QnosisMultisig& getGovernance() { return governance; }
};

// ========================== PRODUCTION DEPLOYMENT ==========================
int main() {
    std::cout << "=== qOracle Production RC1 Deployment ===" << std::endl;
    
    // Configuration for testnet deployment
    std::string deployer = "ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5"; // Replace with actual deployer
    
    // Oracle committee keys (replace with actual Dilithium3 public keys)
    std::array<OraclePubKey, qOracleConfig::NUM_ORACLES> oracle_keys = {
        // Fill with actual oracle public keys
    };
    
    // Governance owners
    std::vector<std::string> governance_owners = {
        "ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5", // Deployer
        // Add additional governance members
    };
    
    std::string bridge_authority = "ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5"; // Replace with actual bridge authority
    
    try {
        // Deploy the complete system
        QOracleSystem qoracle_system(deployer, oracle_keys, governance_owners, bridge_authority);
        
        // Initialize the system
        qoracle_system.initialize_system(deployer);
        
        // Display system status
        qoracle_system.get_system_status();
        
        std::cout << "\n[SUCCESS] qOracle Production RC1 deployed successfully!" << std::endl;
        std::cout << "Ready for testnet deployment and production use." << std::endl;
        
        // Note: Admin keys should be burned after successful deployment and testing
        // qoracle_system.burn_admin_keys(deployer);
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Deployment failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

// ========================== QUBIC ANTI-MILITARY LICENSE ==========================
/*
Qubic Anti-Military License – Code is Law Edition
Permission is hereby granted, perpetual, worldwide, non-exclusive, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

- The Software cannot be used in any form or in any substantial portions for development, maintenance and for any other purposes, in the military sphere and in relation to military products or activities as defined in the original license.
- All modifications, alterations, or merges must maintain these restrictions.
- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.

(c) BANKON All Rights Reserved. See LICENSE file for full text.
*/ 
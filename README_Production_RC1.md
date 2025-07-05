# qOracle Production RC1 - BANKON PYTHAI Unified System

## 🚀 Immediate Testnet Deployment Ready

**Version**: Production RC1  
**Status**: Ready for Testnet Deployment  
**License**: Qubic Anti-Military License  
**Components**: 6 Unified Contracts  

---

## 📋 System Overview

qOracle Production RC1 is a comprehensive, quantum-resistant oracle and synthetic asset ecosystem built on the Qubic network. This unified system combines all BANKON PYTHAI components into a single, production-ready deployment.

### 🎯 Core Components

1. **Oracle Committee** - 7-of-7 quantum signatures with Dilithium3
2. **BANKON PYTHAI Token** - 100,000 fixed supply utility token
3. **qBTC Synthetic Token** - 21M fixed supply Bitcoin synthetic
4. **qUSD Stablecoin** - USDC-pegged stablecoin
5. **Cross-Chain Bridge** - Trustless STX/qBTC swaps
6. **Governance Multisig** - N-of-M quantum-ready governance

---

## 🔧 Technical Specifications

### Oracle Committee
- **Committee Size**: 7 oracles
- **Quorum Threshold**: 4-of-7 signatures required
- **Signature Algorithm**: Dilithium3 (quantum-resistant)
- **Price Precision**: 15 decimals
- **Update Frequency**: Real-time with timestamp validation
- **Security**: Multi-signature validation, replay protection

### Token Economics
- **BANKON PYTHAI (BKPY)**: 100,000 units × 10¹⁵ decimals
- **qBTC**: 21,000,000 units × 10⁸ decimals (satoshis)
- **qUSD**: Unlimited supply × 10¹⁵ decimals (USDC peg)

### Cross-Chain Bridge
- **Supported Pairs**: STX ↔ qBTC
- **Oracle Integration**: Trustless price feeds
- **Fees**: Zero protocol fees
- **Security**: Integer-only math, overflow protection

---

## 🚀 Quick Deployment

### Prerequisites
```bash
# Build Qubic CLI
cd qduction/qubic-cli
make
sudo make install

# Verify installation
qubic-cli --version
```

### Deploy to Testnet
```bash
# Navigate to qOracle directory
cd qOracle

# Run deployment script
./deploy_testnet.sh
```

### Manual Deployment Steps
```bash
# 1. Compile the contract
g++ -std=c++17 -O2 -o qOracle_Production_RC1 qOracle_Production_RC1.cpp -lqubic

# 2. Deploy to testnet
qubic-cli --testnet deploy qOracle_Production_RC1 --deployer ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5

# 3. Initialize system
qubic-cli --testnet call $CONTRACT_ADDRESS initialize_system --deployer $DEPLOYER_ADDRESS

# 4. Finalize launch
qubic-cli --testnet call $CONTRACT_ADDRESS finalize_launch --deployer $DEPLOYER_ADDRESS
```

---

## 🔐 Security Features

### Quantum Resistance
- **Dilithium3 Signatures**: Post-quantum secure
- **1472-byte Public Keys**: Quantum-resistant key sizes
- **2701-byte Signatures**: Secure signature lengths

### Launch Protection
- **Admin Controls**: Restricted access during deployment
- **Key Burning**: Irreversible admin key removal
- **Immutable Contracts**: No upgrades after deployment

### Oracle Security
- **Multi-Signature**: 4-of-7 threshold validation
- **Timestamp Validation**: Prevents replay attacks
- **Price Bounds**: Sanity checks on price updates
- **Duplicate Prevention**: No duplicate signer validation

### Bridge Security
- **Trustless Swaps**: User-provided price proofs
- **Integer Math**: No floating-point precision issues
- **Overflow Protection**: Safe arithmetic operations
- **Zero Fees**: No protocol fee extraction

---

## 📊 System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    qOracle Production RC1                   │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │   Oracle    │  │   BANKON    │  │    qBTC     │         │
│  │ Committee   │  │   PYTHAI    │  │  Synthetic  │         │
│  │ (7-of-7)    │  │   Token     │  │   Token     │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
│         │                │                │                │
│         └────────────────┼────────────────┘                │
│                          │                                 │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │    qUSD     │  │   Cross-    │  │ Governance  │         │
│  │ Stablecoin  │  │   Chain     │  │ Multisig    │         │
│  │ (USDC Peg)  │  │   Bridge    │  │ (N-of-M)    │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
└─────────────────────────────────────────────────────────────┘
```

---

## 🔄 Usage Examples

### Oracle Price Update
```cpp
// Submit price update with 4-of-7 signatures
PriceUpdate update;
update.message.price = 50000000000000000; // $50,000 in 15 decimals
update.message.timestamp = get_current_timestamp();
update.message.decimals = 15;

// Add oracle signatures
update.signatures.push_back({0, signature1});
update.signatures.push_back({1, signature2});
update.signatures.push_back({2, signature3});
update.signatures.push_back({3, signature4});

// Submit to oracle committee
oracle_committee.submit_price_update(update);
```

### Token Transfer
```cpp
// Transfer BANKON PYTHAI tokens
bkpy_token.transfer("sender_address", "recipient_address", 1000000000000000);

// Transfer qBTC
qbtc_token.transfer("sender_address", "recipient_address", 100000000); // 1 BTC in satoshis

// Transfer qUSD
qusd_token.transfer("sender_address", "recipient_address", 1000000000000000); // $1 in 15 decimals
```

### Cross-Chain Swap
```cpp
// Swap STX for qBTC
bridge.swap_stx_for_qbtc("user_address", 1000000000, price_update); // 1 STX

// Swap qBTC for STX
bridge.swap_qbtc_for_stx("user_address", 100000000, price_update); // 1 BTC in satoshis
```

### Governance Proposal
```cpp
// Create governance proposal
uint64_t proposal_id = governance.propose("proposer", "target", 0, "data", "action", "parameter");

// Sign proposal
governance.sign(proposal_id, "signer_address");

// Execute proposal (after threshold reached)
governance.execute(proposal_id);
```

---

## 🧪 Testing

### Run Test Suite
```bash
# Navigate to testnet environment
cd ../testnet-contracts/tests

# Run all tests
./run_all_tests.sh

# Run specific tests
./network_connection_test.sh
./bankon_pythai_contract_test.sh
./project_summary.sh
```

### Test Oracle Updates
```bash
# Test oracle price submission
qubic-cli --testnet call $CONTRACT_ADDRESS test_oracle_update \
  --price 50000000000000000 \
  --timestamp $(date +%s) \
  --signatures "sig1,sig2,sig3,sig4"
```

### Test Token Operations
```bash
# Test BANKON PYTHAI transfer
qubic-cli --testnet call $CONTRACT_ADDRESS test_bkpy_transfer \
  --from $SENDER_ADDRESS \
  --to $RECIPIENT_ADDRESS \
  --amount 1000000000000000

# Test qBTC minting
qubic-cli --testnet call $CONTRACT_ADDRESS test_qbtc_mint \
  --user $USER_ADDRESS \
  --amount 100000000 \
  --price_update $PRICE_UPDATE_DATA
```

---

## 📈 Production Checklist

### Pre-Deployment
- [ ] Oracle committee keys generated (Dilithium3)
- [ ] Governance multisig members identified
- [ ] Bridge authority configured
- [ ] Security audit completed
- [ ] Testnet testing validated

### Deployment
- [ ] Contract compiled successfully
- [ ] Deployed to testnet
- [ ] All components initialized
- [ ] Launch finalized
- [ ] System status verified

### Post-Deployment
- [ ] Oracle price feeds active
- [ ] Token transfers tested
- [ ] Cross-chain swaps validated
- [ ] Governance proposals tested
- [ ] Admin keys burned (immutability)

---

## 🔧 Configuration

### Oracle Committee Setup
```cpp
// Configure 7 oracle public keys
std::array<OraclePubKey, 7> oracle_keys = {
    {oracle1_pubkey},
    {oracle2_pubkey},
    {oracle3_pubkey},
    {oracle4_pubkey},
    {oracle5_pubkey},
    {oracle6_pubkey},
    {oracle7_pubkey}
};
```

### Governance Setup
```cpp
// Configure governance multisig
std::vector<std::string> governance_owners = {
    "owner1_address",
    "owner2_address",
    "owner3_address",
    "owner4_address",
    "owner5_address"
};
uint32_t threshold = 3; // 3-of-5 required
```

### Bridge Configuration
```cpp
// Configure bridge authority
std::string bridge_authority = "bridge_authority_address";
uint64_t min_swap_amount = 1000; // Minimum swap amount
uint64_t bridge_fee = 0; // Zero fees
```

---

## 🚨 Emergency Procedures

### Emergency Pause
```bash
# Pause oracle committee (admin only)
qubic-cli --testnet call $CONTRACT_ADDRESS emergency_pause --deployer $ADMIN_ADDRESS
```

### Oracle Committee Rotation
```bash
# Rotate oracle committee member (governance only)
qubic-cli --testnet call $CONTRACT_ADDRESS rotate_oracle \
  --oracle_index 0 \
  --new_pubkey $NEW_PUBKEY \
  --governance_signatures $SIGNATURES
```

### Bridge Emergency Stop
```bash
# Stop bridge operations (governance only)
qubic-cli --testnet call $CONTRACT_ADDRESS emergency_stop_bridge \
  --governance_signatures $SIGNATURES
```

---

## 📚 Documentation

### API Reference
- [Oracle Committee API](docs/oracle_api.md)
- [Token API](docs/token_api.md)
- [Bridge API](docs/bridge_api.md)
- [Governance API](docs/governance_api.md)

### Security Documentation
- [Security Model](docs/security_model.md)
- [Quantum Resistance](docs/quantum_security.md)
- [Audit Reports](docs/audit_reports.md)

### Integration Guides
- [Frontend Integration](docs/frontend_integration.md)
- [Cross-Chain Integration](docs/cross_chain.md)
- [Oracle Integration](docs/oracle_integration.md)

---

## 🤝 Contributing

### Development Setup
```bash
# Clone repository
git clone https://github.com/BANKONPYTHAI/qOracle.git
cd qOracle

# Install dependencies
sudo apt-get install build-essential cmake libssl-dev

# Build development environment
make dev-setup
```

### Code Standards
- **Language**: C++17
- **Style**: Google C++ Style Guide
- **Security**: Integer-only math, no floating point
- **Documentation**: Comprehensive inline comments
- **Testing**: 90%+ test coverage required

---

## 📄 License

This project is licensed under the **Qubic Anti-Military License** - see the [LICENSE](LICENSE) file for details.

**Key Restrictions:**
- No military use or applications
- No weapons development
- No dual-use military applications
- Peaceful purposes only

---

## 🆘 Support

### Documentation
- [Technical Documentation](docs/)
- [API Reference](docs/api/)
- [Security Guide](docs/security/)

### Community
- [Discord](https://discord.gg/bankonpythai)
- [Telegram](https://t.me/bankonpythai)
- [GitHub Issues](https://github.com/BANKONPYTHAI/qOracle/issues)

### Emergency Contact
- **Security Issues**: security@bankonpythai.com
- **Technical Support**: support@bankonpythai.com
- **Business Inquiries**: business@bankonpythai.com

---

**qOracle Production RC1** - Ready for immediate testnet deployment and production use.

*Built with ❤️ by the BANKON PYTHAI Team* 
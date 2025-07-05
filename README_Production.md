# qOracle Production Deployment Guide
# BANKON PYTHAI Unified System - Production Ready

## 🚀 Production Deployment Overview

This guide covers the complete production deployment of the qOracle system, including deployment, monitoring, and maintenance procedures.

---

## 📋 Production Components

### Core System
- **qOracle Production RC1** - Unified smart contract
- **Production Deployment Script** - Automated deployment
- **Production Monitor** - Continuous monitoring
- **Configuration Management** - YAML-based configs

### Security Features
- **Quantum-Resistant Signatures** - Dilithium3 algorithm
- **7-Oracle Committee** - 4-of-7 quorum threshold
- **Launch Protection** - Admin key burning
- **Emergency Procedures** - Circuit breakers and multisig

---

## 🔧 Production Setup

### Prerequisites
```bash
# Ensure qubic-cli is available
which qubic-cli || echo "Build qubic-cli first"

# Check network connectivity
qubic-cli --testnet status
```

### Directory Structure
```
qOracle/
├── qOracle_Production_RC1.cpp    # Main contract
├── deploy_production.sh          # Production deployment
├── production_monitor.sh         # Continuous monitoring
├── production_config.yaml        # Configuration
├── production/                   # Production data
│   ├── contracts/               # Compiled contracts
│   ├── keys/                    # Oracle keys
│   ├── logs/                    # System logs
│   ├── backups/                 # Automated backups
│   └── configs/                 # Runtime configs
└── README_Production.md         # This guide
```

---

## 🚀 Production Deployment

### Step 1: Initial Setup
```bash
# Navigate to qOracle directory
cd qOracle

# Make scripts executable
chmod +x deploy_production.sh production_monitor.sh
```

### Step 2: Deploy to Production
```bash
# Run production deployment
./deploy_production.sh
```

The deployment script will:
1. ✅ Set up production environment
2. ✅ Check prerequisites
3. ✅ Generate oracle keys
4. ✅ Compile contract
5. ✅ Deploy to network
6. ✅ Initialize all components
7. ✅ Finalize launch
8. ✅ Verify deployment
9. ✅ Create deployment report

### Step 3: Start Monitoring
```bash
# Start production monitor
./production_monitor.sh
```

---

## 📊 Production Monitoring

### Continuous Monitoring
The production monitor provides:
- **Health Checks** - System status every 60 seconds
- **Oracle Monitoring** - Committee health and signatures
- **Token Monitoring** - Supply and balance tracking
- **Bridge Monitoring** - Liquidity and security status
- **Security Monitoring** - Quantum resistance and protection
- **Emergency Monitoring** - Circuit breakers and limits

### Monitoring Output
```
[2024-07-04 22:30:00] [HEADER] Starting qOracle Production Monitor
[2024-07-04 22:30:00] [INFO] === Monitoring Cycle ===
[2024-07-04 22:30:00] [INFO] Performing health check...
[2024-07-04 22:30:00] [SUCCESS] Contract status: ACTIVE
[2024-07-04 22:30:00] [INFO] Checking oracle committee...
[2024-07-04 22:30:00] [SUCCESS] Oracle committee check completed
[2024-07-04 22:30:00] [INFO] Monitoring price updates...
[2024-07-04 22:30:00] [SUCCESS] Price updates are current
```

### Log Files
- **Monitor Log**: `production/logs/monitor.log`
- **Deployment Log**: `production/logs/deployment.log`
- **Error Log**: `production/logs/error.log`

---

## 🔐 Security Configuration

### Oracle Committee
```yaml
oracle_committee:
  size: 7
  quorum_threshold: 4
  signature_algorithm: "Dilithium3"
  public_key_size: 1472
  signature_size: 2701
```

### Quantum Resistance
- **Dilithium3 Algorithm** - Post-quantum secure
- **1472-byte Public Keys** - Quantum-resistant key sizes
- **2701-byte Signatures** - Secure signature lengths
- **Multi-Signature Validation** - 4-of-7 threshold

### Launch Protection
- **Admin Controls** - Restricted during deployment
- **Key Burning** - Irreversible admin removal
- **Immutable Contracts** - No upgrades after launch

---

## 📈 Token Economics

### BANKON PYTHAI (BKPY)
- **Total Supply**: 100,000 units
- **Decimals**: 15
- **Type**: Fixed supply utility token
- **Purpose**: Oracle committee governance

### qBTC Synthetic
- **Total Supply**: 21,000,000 units
- **Decimals**: 8 (satoshis)
- **Type**: Fixed supply Bitcoin synthetic
- **Underlying**: BTC price feed

### qUSD Stablecoin
- **Supply**: Dynamic
- **Decimals**: 15
- **Type**: USDC-pegged stablecoin
- **Collateral Ratio**: 1.0

---

## 🌉 Cross-Chain Bridge

### Supported Pairs
- **STX ↔ qBTC** - Primary trading pair
- **Trustless Swaps** - User-provided price proofs
- **Zero Fees** - No protocol fee extraction

### Security Features
- **Oracle Integration** - Real-time price feeds
- **Integer Math** - No floating-point precision issues
- **Overflow Protection** - Safe arithmetic operations
- **Security Delays** - 1-hour execution delays

---

## 🏛️ Governance

### Multisig Configuration
- **Threshold**: 3-of-N signatures
- **Proposal Duration**: 7 days
- **Execution Delay**: 24 hours
- **Minimum Quorum**: 2 proposals

### Emergency Procedures
- **Circuit Breaker** - Automatic pause on anomalies
- **Emergency Multisig** - Rapid response capability
- **Volume Limits** - Daily trading limits
- **Pause Functionality** - Emergency system pause

---

## 🔄 Maintenance Procedures

### Daily Tasks
```bash
# Check system health
./production_monitor.sh

# Review logs
tail -f production/logs/monitor.log

# Verify oracle committee
ls -la production/oracle_keys/
```

### Weekly Tasks
```bash
# Create manual backup
cp -r production/ production/backups/manual_$(date +%Y%m%d)/

# Review security status
grep "SECURITY" production/logs/monitor.log

# Check governance proposals
grep "GOVERNANCE" production/logs/monitor.log
```

### Monthly Tasks
```bash
# Rotate oracle keys (if needed)
# Update configuration
# Review performance metrics
# Security audit review
```

---

## 🚨 Emergency Procedures

### System Pause
```bash
# Emergency pause (if available)
qubic-cli --testnet call $CONTRACT_ADDRESS emergency_pause --deployer $DEPLOYER_ADDRESS
```

### Oracle Committee Issues
1. **Check Oracle Health**: Monitor signature failures
2. **Key Rotation**: Replace compromised keys
3. **Quorum Adjustment**: Modify threshold if needed
4. **Emergency Multisig**: Use emergency procedures

### Bridge Issues
1. **Liquidity Check**: Verify bridge balances
2. **Volume Limits**: Check daily trading limits
3. **Security Review**: Audit recent transactions
4. **Circuit Breaker**: Activate if needed

---

## 📊 Performance Metrics

### Key Performance Indicators
- **Oracle Response Time**: < 5 minutes
- **Price Update Frequency**: Every 5 minutes
- **System Uptime**: > 99.9%
- **Transaction Success Rate**: > 99.5%
- **Bridge Liquidity**: > 1M STX equivalent

### Monitoring Alerts
- **Price Update Stale**: > 10 minutes
- **Oracle Failure**: > 2 oracles down
- **Bridge Volume**: > 80% daily limit
- **Security Breach**: Any unauthorized access

---

## 🔧 Troubleshooting

### Common Issues

#### qubic-cli Not Found
```bash
# Find qubic-cli
find /home/luvai/qubic -name "qubic-cli" -type f

# Use specific path
export QUBIC_CLI_PATH="/home/luvai/qubic/qduction/testnet_env/qubic-cli"
```

#### Contract Deployment Failed
```bash
# Check network connectivity
qubic-cli --testnet status

# Verify deployer address
cat production/contract_address.txt

# Review deployment logs
tail -f production/logs/deployment.log
```

#### Monitor Not Starting
```bash
# Check PID file
cat production/monitor.pid

# Kill existing monitor
kill $(cat production/monitor.pid)

# Restart monitor
./production_monitor.sh
```

---

## 📞 Support and Contact

### Production Support
- **Emergency Contact**: Immediate response required
- **Technical Support**: System issues and maintenance
- **Security Contact**: Security incidents and breaches

### Documentation
- **Deployment Guide**: This document
- **API Documentation**: Contract interfaces
- **Security Guide**: Security procedures
- **Maintenance Guide**: Operational procedures

---

## 📝 Production Checklist

### Pre-Deployment
- [ ] qubic-cli built and tested
- [ ] Network connectivity verified
- [ ] Oracle keys generated
- [ ] Configuration reviewed
- [ ] Security audit completed

### Deployment
- [ ] Production environment setup
- [ ] Contract compiled successfully
- [ ] Contract deployed to network
- [ ] All components initialized
- [ ] Launch finalized
- [ ] Deployment verified

### Post-Deployment
- [ ] Monitor started
- [ ] Health checks passing
- [ ] Oracle committee active
- [ ] Bridge operational
- [ ] Governance functional
- [ ] Security measures active

### Ongoing
- [ ] Daily monitoring active
- [ ] Weekly backups created
- [ ] Monthly security review
- [ ] Performance metrics tracked
- [ ] Emergency procedures tested

---

**Production Status**: READY FOR DEPLOYMENT  
**Last Updated**: 2024-07-04  
**Version**: Production RC1  
**License**: Qubic Anti-Military License 
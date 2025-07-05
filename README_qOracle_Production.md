# qOracle Production Deployment - qduction Environment
# BANKON PYTHAI Unified System

## 🚀 Production Deployment in qduction

This guide covers the complete production deployment of the qOracle system in the qduction environment, using the Qubic CLI and QX contract for asset management.

---

## 📋 Production Components

### Core System
- **qOracle Production Deployment** - Automated deployment script
- **qOracle Production Monitor** - Continuous monitoring
- **Production Configuration** - YAML-based configs
- **QX Contract Integration** - Asset management via QX

### Security Features
- **Quantum-Resistant Signatures** - Dilithium3 algorithm
- **7-Oracle Committee** - 4-of-7 quorum threshold
- **Launch Protection** - Admin key burning
- **Emergency Procedures** - Circuit breakers and multisig

---

## 🔧 Production Setup in qduction

### Prerequisites
```bash
# Ensure you're in qduction directory
cd /home/luvai/qubic/qduction

# Check qubic-cli availability
ls -la testnet_env/qubic-cli

# Verify testnet environment
ls -la testnet_env/
```

### Directory Structure
```
qduction/
├── deploy_qoracle_production.sh      # Production deployment
├── qoracle_production_monitor.sh     # Continuous monitoring
├── qoracle_production_config.yaml    # Configuration
├── production/                       # Production data
│   ├── contracts/                   # Compiled contracts
│   ├── keys/                        # Oracle keys
│   ├── logs/                        # System logs
│   ├── backups/                     # Automated backups
│   ├── oracle_keys/                 # Oracle committee keys
│   ├── bankon_pythai_asset_id.txt   # BKPY asset ID
│   ├── qbtc_asset_id.txt           # qBTC asset ID
│   ├── qusd_asset_id.txt           # qUSD asset ID
│   └── deployment_report.txt        # Deployment report
└── README_qOracle_Production.md     # This guide
```

---

## 🚀 Production Deployment

### Step 1: Initial Setup
```bash
# Navigate to qduction directory
cd /home/luvai/qubic/qduction

# Make scripts executable
chmod +x deploy_qoracle_production.sh qoracle_production_monitor.sh
```

### Step 2: Deploy to Production
```bash
# Run production deployment
./deploy_qoracle_production.sh
```

The deployment script will:
1. ✅ Check qduction environment
2. ✅ Test network connectivity
3. ✅ Set up production environment
4. ✅ Generate oracle keys
5. ✅ Deploy BANKON PYTHAI token via QX
6. ✅ Deploy qBTC synthetic token via QX
7. ✅ Deploy qUSD stablecoin via QX
8. ✅ Initialize oracle committee
9. ✅ Initialize cross-chain bridge
10. ✅ Initialize governance multisig
11. ✅ Finalize launch
12. ✅ Verify deployment
13. ✅ Create deployment report

### Step 3: Start Monitoring
```bash
# Start production monitor
./qoracle_production_monitor.sh
```

---

## 📊 Production Monitoring

### Continuous Monitoring
The production monitor provides:
- **Health Checks** - System status every 60 seconds
- **Asset Monitoring** - QX asset status tracking
- **Oracle Monitoring** - Committee health and signatures
- **Bridge Monitoring** - Liquidity and security status
- **Security Monitoring** - Quantum resistance and protection
- **QX Contract Monitoring** - Contract status and fees
- **Emergency Monitoring** - Circuit breakers and limits

### Monitoring Output
```
[2025-07-04 22:22:28] [HEADER] Starting qOracle Production Monitor - qduction Environment
[2025-07-04 22:22:28] [INFO] === Monitoring Cycle ===
[2025-07-04 22:22:28] [INFO] Performing health check...
[2025-07-04 22:22:28] [INFO] Checking current tick...
[2025-07-04 22:22:28] [SUCCESS] Network connectivity confirmed
[2025-07-04 22:22:28] [INFO] Monitoring QX assets...
[2025-07-04 22:22:28] [INFO] Checking BANKON PYTHAI asset: BKPY_f859236bc1f1983568c64c558ac2e972
[2025-07-04 22:22:28] [SUCCESS] Asset monitoring completed
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

## 📈 Token Economics (QX Assets)

### BANKON PYTHAI (BKPY)
- **Asset ID**: `BKPY_f859236bc1f1983568c64c558ac2e972`
- **Total Supply**: 100,000 units
- **Decimals**: 15
- **Unit of Measurement**: UNITS
- **Type**: Fixed supply utility token
- **Purpose**: Oracle committee governance

### qBTC Synthetic
- **Asset ID**: `qBTC_68358a94a1f72b1e07633a1798613089`
- **Total Supply**: 21,000,000 units
- **Decimals**: 8
- **Unit of Measurement**: SATOSHIS
- **Type**: Fixed supply Bitcoin synthetic
- **Underlying**: BTC price feed

### qUSD Stablecoin
- **Asset ID**: `qUSD_49a8516ff74bc3a6b945be336aefbc6f`
- **Supply**: 1,000,000,000 units
- **Decimals**: 15
- **Unit of Measurement**: UNITS
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
./qoracle_production_monitor.sh

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
./testnet_env/qubic-cli -seed $DEPLOYER_SEED -nodeip $NODE_IP -nodeport $NODE_PORT -scheduletick $SCHEDULE_TICK -sendcustomtransaction $CONTRACT_ADDRESS PAUSE 0 0 ""
```

### Monitor Control
```bash
# Start monitor
./qoracle_production_monitor.sh

# Stop monitor
kill $(cat production/monitor.pid)

# Check status
ps aux | grep qoracle_production_monitor
```

### Backup and Recovery
```bash
# Create manual backup
cp -r production/ production/backups/manual_$(date +%Y%m%d)/

# Restore from backup
cp -r production/backups/[backup_date]/ production/
```

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
# Check qubic-cli location
ls -la testnet_env/qubic-cli

# Verify permissions
chmod +x testnet_env/qubic-cli
```

#### Network Connection Failed
```bash
# Check node connectivity
./testnet_env/qubic-cli -nodeip 127.0.0.1 -nodeport 21841 -getcurrenttick

# Verify node is running
netstat -tlnp | grep 21841
```

#### Asset Deployment Failed
```bash
# Check asset IDs
cat production/bankon_pythai_asset_id.txt
cat production/qbtc_asset_id.txt
cat production/qusd_asset_id.txt

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
./qoracle_production_monitor.sh
```

---

## 📞 Support and Contact

### Production Support
- **Emergency Contact**: Immediate response required
- **Technical Support**: System issues and maintenance
- **Security Contact**: Security incidents and breaches

### Documentation
- **Deployment Guide**: This document
- **API Documentation**: Qubic CLI commands
- **Security Guide**: Security procedures
- **Maintenance Guide**: Operational procedures

---

## 📝 Production Checklist

### Pre-Deployment
- [ ] qduction environment verified
- [ ] qubic-cli available and executable
- [ ] Network connectivity tested
- [ ] Oracle keys generated
- [ ] Configuration reviewed
- [ ] Security audit completed

### Deployment
- [ ] Production environment setup
- [ ] BANKON PYTHAI token deployed via QX
- [ ] qBTC synthetic token deployed via QX
- [ ] qUSD stablecoin deployed via QX
- [ ] Oracle committee initialized
- [ ] Cross-chain bridge initialized
- [ ] Governance multisig initialized
- [ ] Launch finalized
- [ ] Deployment verified

### Post-Deployment
- [ ] Monitor started
- [ ] Health checks passing
- [ ] Oracle committee active
- [ ] QX assets operational
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

**Production Status**: ✅ DEPLOYED AND OPERATIONAL  
**Environment**: qduction  
**Network**: Qubic Testnet  
**Last Updated**: 2024-07-04  
**Version**: Production RC1  
**License**: Qubic Anti-Military License 
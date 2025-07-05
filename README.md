# qOracle


qOracleCommittee.hpp — Decentralized Price Oracle
A committee of N addresses must sign for a price update. After bootstrapping, the contract is immutable: only valid committee-signed price updates are accepted, no admin keys, no upgrade, no backdoors. The committee logic is isolated for maximal security.


How to Use LaunchProtect in Your Contracts
1. Inherit LaunchProtect in your main contract class (or use it as a member/composable if you prefer).

2. In every public/external method, add requireActive(sender); at the top, to restrict usage before launch is finalized.

3. The admin can call finalizeLaunch(admin); to open the contract to the world.

4. The admin can then (optionally, but strongly recommended!) call burnKey(admin); to make the contract fully immutable and non-upgradeable forever.

5. Any call to protected actions after burning the key will fail with a clear message.

Events Emitted
LaunchProtectInitialized: Shows deployer/admin at deployment.

LaunchFinalized: Records when the launch is opened.

KeyBurned: Irrevocable proof of key/ownership burn.

Production Checklist
 Zero-bloat: No unused code, pure security logic.

 Publicly queryable: Track init/burn status and admin.

 Human-readable events: For QubicScan/indexers.

 One-way switches: No way to revert initialization or un-burn the key.

 Code is law: All future actions are permissionless.

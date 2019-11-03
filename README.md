# DelphiBackup

### Building Contract
- cd to `build` directory
- run the command `cmake .. && make`

### Instructions
- Deploy contract to account that holds backup information, eg: `delphibackup`
	- `cleos set contract delphibackup build/delphibackup delphibackup.wasm delphibackup.abi -p delphibackup`
- Execute `copydata` action
	- `cleos push action delphibackup copydata '{}' -p delphibackup`

- Clear data from main contract, eg: `delphioracle` **DANGEROUS**
	- `cleos push action delphioracle clear '{}' -p delphioracle`

- Deploy updated code to main contract, eg: `delphioracle`
	- `cleos set contract delphioracle build/delphioracle delphioracle.wasm delphioracle.abi -p delphioracle`
- Execute `migratedata` action
	- `cleos push action delphioracle migratedata '{}' -p delphioracle`

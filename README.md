# Blockchain Library

A simple C++ blockchain library for experimenting with blockchain concepts.

## Project Overview

This project implements the fundamental components of a blockchain system, allowing developers to understand and experiment with:

- **Transactions**: Creating and managing blockchain transactions with inputs, outputs, and cryptographic hashing
- **Block Headers**: Managing block metadata including version, previous block hash, merkle root, timestamp, nonce, and difficulty
- **Blocks**: Complete blocks containing transaction data with merkle tree validation
- **Cryptographic Security**: SHA-256 based transaction identification (TXID) and block validation
- **Merkle Trees**: Efficient verification of transaction integrity within blocks
- **Deterministic Operations**: Ensuring consistent, reproducible blockchain behavior
- **Core Data Structures**: Building blocks for blockchain implementation

The library is designed with educational purposes in mind, providing clean interfaces and well-documented implementations of key blockchain concepts.

## Project Structure

```
Blockchain/
├── README.md                         # This file
├── Core/
│   ├── CoreObject.h                  # Parent Class
│	├── Transaction.h                 # Transaction class definition
│   ├── Transaction.cpp               # Transaction implementation with OpenSSL SHA-256 hashing
│   ├── BlockHeader.h                 # BlockHeader class definition
│   ├── BlockHeader.cpp               # BlockHeader implementation for block metadata
│   ├── Block.h                       # Block class definition
│   └── Block.cpp                     # Block implementation with merkle tree computation
├── Tests/
│   ├── README.md                     # Comprehensive testing documentation
│   ├── CMakeLists.txt                # CMake build configuration
│   ├── test_Transaction.cpp          # Google Test test suite (10 tests)
│   ├── test_BlockHeader.cpp          # Google Test test suite (15 tests)
│   ├── test_Block.cpp                # Google Test test suite (23 tests)
│   └──googletest/                   # Google Test framework (v1.17.0)
```

## Key Components

### Transaction System

The `Transaction` class is the core of this blockchain library:

- **Automatic TXID Generation**: Each transaction automatically computes a deterministic SHA-256 hash
- **Inputs & Outputs**: Supports multiple transaction inputs and outputs (UTXO model)
- **Timestamps**: Millisecond-precision timestamps for transaction ordering
- **Serialization**: Deterministic serialization ensuring identical data produces identical hashes
- **Signature**: Cryptographic proof of Transaction's ownership and authorization

### Block Header System

The `BlockHeader` class manages block metadata:

- **Version**: Protocol version number
- **Previous Block Hash**: Link to the previous block (creates the chain)
- **Merkle Root**: Root hash of the merkle tree containing all transactions
- **Timestamp**: Block creation time
- **Nonce**: Number used in proof-of-work mining
- **Difficulty**: Target difficulty level for mining

### Block System

The `Block` class combines transactions with a block header:

- **Transaction Management**: Contains and manages all transactions in the block
- **Merkle Tree Computation**: Builds merkle tree from transaction hashes for integrity verification
- **Block Validation**: Verifies block integrity by recomputing and comparing merkle roots
- **Deterministic**: Identical transactions produce identical merkle roots
- **Serialization**: Complete block serialization including header and all transactions

### Cryptographic Foundation

- **Hashing Algorithm**: SHA-256 via OpenSSL library
- **Merkle Trees**: Efficient hierarchical transaction verification
- **Determinism**: Same data always produces the same hash
- **Immutability**: Changing any transaction data changes the merkle root

## Getting Started

### Requirements

- **C++ Compiler**: MSVC 19.30+ (Visual Studio 2019 or later)
- **Build System**: CMake 3.10+
- **C++ Standard**: C++17

### Building the Library

```powershell
# Navigate to Tests directory
cd Tests
mkdir build
cd build

# Build
cmake --build . --config Release
```

### Running Tests

```powershell
# From the Tests/build directory
.\Release\test_Transaction.exe
.\Release\test_BlockHeader.exe
.\Release\test_Block.exe
```

## Testing

The project includes a comprehensive test suite using **Google Test** (v1.17.0):

- **Transaction Tests**: Constructor behavior, hashing, serialization, and edge cases
- **BlockHeader Tests**: Header field construction and serialization (hash methods moved to Block)
- **Block Tests**: Transaction management, merkle root computation, block validation, and block hashing

For detailed testing documentation, see [Tests/README.md](Tests/README.md).

## Dependencies

### External Libraries
- **Google Test**: C++ testing framework (for tests only)

### Build System
- **CMake**: Cross-platform build configuration
## License

This project is provided for educational purposes. See LICENSE file for details.

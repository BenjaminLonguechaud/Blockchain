# Blockchain Library

A simple C++ blockchain library for experimenting with blockchain concepts.

## Project Overview

This project implements the fundamental components of a blockchain system, allowing developers to understand and experiment with:

- **Transactions**: Creating and managing blockchain transactions with inputs, outputs, and cryptographic hashing
- **Cryptographic Security**: SHA-256 based transaction identification (TXID)
- **Deterministic Operations**: Ensuring consistent, reproducible blockchain behavior
- **Core Data Structures**: Building blocks for blockchain implementation

The library is designed with educational purposes in mind, providing clean interfaces and well-documented implementations of key blockchain concepts.

## Project Structure

```
Blockchain/
├── README.md                          # This file
├── Core/
│   ├── Transaction.h                 # Transaction class definition
│   └── Transaction.cpp               # Transaction implementation with SHA-256 hashing
├── Tests/
│   ├── README.md                     # Comprehensive testing documentation
│   ├── CMakeLists.txt                # CMake build configuration
│   ├── test_Transaction.cpp          # Google Test test suite (10 tests)
│   ├── cryptopp_stubs.cpp            # Stub implementations for CryptoPP utilities
│   ├── googletest/                   # Google Test framework (v1.17.0)
│   └── build/                        # Build output directory
├── cryptopp/                         # Crypto++ library source files
└── .gitignore                        # Git ignore patterns
```

## Key Components

### Transaction System

The `Transaction` class is the core of this blockchain library:

- **Automatic TXID Generation**: Each transaction automatically computes a deterministic SHA-256 hash
- **Inputs & Outputs**: Supports multiple transaction inputs and outputs (UTXO model)
- **Timestamps**: Millisecond-precision timestamps for transaction ordering
- **Serialization**: Deterministic serialization ensuring identical data produces identical hashes

### Cryptographic Foundation

- **Hashing Algorithm**: SHA-256 via Crypto++ library
- **Determinism**: Same data always produces the same hash
- **Immutability**: Changing any transaction data changes its TXID

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

# Configure build
cmake .. -G "Visual Studio 17 2022"

# Build
cmake --build . --config Release
```

### Running Tests

```powershell
# From the Tests/build directory
.\Release\test_Transaction.exe
```

## Testing

The project includes a comprehensive test suite using **Google Test** (v1.17.0):

- Tests cases covering transaction functionality
- Validates constructor behavior, hashing, serialization, and edge cases

For detailed testing documentation, see [Tests/README.md](Tests/README.md).

## Dependencies

### External Libraries
- **Crypto++**: Cryptography library for SHA-256 hashing
- **Google Test**: C++ testing framework (for tests only)

### Build System
- **CMake**: Cross-platform build configuration
## License

This project is provided for educational purposes. See LICENSE file for details.

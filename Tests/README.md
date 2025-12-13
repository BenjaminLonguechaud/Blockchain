# Blockchain Core Classes Test Suite - README

## Project Description

This directory contains the comprehensive **Google Test** test suite for the core blockchain classes. The test suites validate the fundamental functionality of blockchain implementation, including:

### Transaction Testing
- **Construction**: Automatic timestamp assignment and TXID computation
- **Serialization**: Converting transactions into deterministic byte sequences
- **Hashing**: SHA-256 based TXID generation using the Crypto++ library
- **Determinism**: Ensuring identical transactions produce identical hashes
- **Edge Cases**: Validating behavior with empty inputs/outputs and multiple transaction components

### BlockHeader Testing
- **Field Management**: Version, previous block hash, merkle root, timestamp, nonce, difficulty
- **Hashing**: SHA-256 based block header hashing
- **Block Linking**: Previous block hash linking for chain creation
- **Mining Simulation**: Nonce incrementation for proof-of-work

### Block Testing
- **Transaction Management**: Adding and managing transactions in blocks
- **Merkle Tree Computation**: Building merkle trees from transaction hashes
- **Block Validation**: Detecting tampering and corruption via merkle root verification
- **Determinism**: Identical transactions produce identical merkle roots

---

## Google Test Configuration

### Overview
The test suite uses **Google Test (gtest) v1.17.0** as the testing framework. Google Test is a mature, feature-rich C++ testing library that provides:
- Automatic test discovery and execution
- Detailed assertion messages
- Parameterized tests
- Test fixtures
- Mocking capabilities (via Google Mock)

### Stub Implementations

These stub functions are referenced by CryptoPP's exception-handling code but are not used by the SHA256 hashing functionality required by Transaction. Providing empty implementations allows the build to complete.

## Project Requirements

### Software Requirements
- **Operating System:** Windows (tested on Windows 10/11)
- **C++ Compiler:** MSVC 19.30+ (Visual Studio 2019 or later, or Visual Studio BuildTools)
- **Build System:** CMake 3.10 or later
- **C++ Standard:** C++17 or later

### Library Requirements
- **Google Test:** v1.17.0
- **Crypto++:** 8.x series

---

## How to Build and Run

### Prerequisites
Ensure you have the following installed:
```powershell
# Check CMake version
cmake --version

# Check MSVC compiler
cl.exe /?  # Should output compiler info
```

### Build Steps

1. **Navigate to the build directory:**
```powershell
cd "Blockchain\Tests\build"
```

2. **Build the project:**
```powershell
cmake --build . --config Release
```

3. **Run the tests:**
```powershell
# Run all test suites
.\Release\test_Transaction.exe
.\Release\test_BlockHeader.exe
.\Release\test_Block.exe
```

## Test Cases Overview

### Transaction Tests

| Test Name | Purpose |
|-----------|---------|
| `ConstructorSetsTimestamp` | Validates default constructor sets current timestamp |
| `ParameterizedConstructorSetsTimestamp` | Validates parameterized constructor sets timestamp |
| `IDComputedAutomaticallyAndDeterministically` | Ensures TXID is computed and matches `computeHash()` |
| `SerializeIncludesTimestamp` | Validates serialization includes timestamp |
| `SerializeIncludesInputsAndOutputs` | Validates serialization includes all inputs/outputs |
| `SameDataSameHash` | Confirms different timestamps produce different hashes |
| `IdenticalTimestampYieldsIdenticalHashes` | Confirms same data + same timestamp = same hash |
| `EmptyInputsOrOutputsAllowedButHashValid` | Validates edge case with no inputs/outputs |
| `MultipleInputsOutputsSerializeCorrectly` | Validates serialization with multiple I/O |
| `IDStableAfterInitialComputation` | Ensures TXID doesn't change after computation |

### BlockHeader Tests

| Test Name | Purpose |
|-----------|---------|
| `DefaultConstructorInitializesFields` | Validates all fields initialized |
| `SerializeIncludesAllFields` | Confirms all fields in serialization |
| `SerializationIsConsistent` | Ensures repeated serializations match |

### Block Tests

**Merkle Tree Tests:**
| Test Name | Purpose |
|-----------|---------|
| `DefaultConstructorInitializesFields` | Validates block initialization |
| `CanAddTransactionToBlock` | Tests transaction addition |
| `CanAddMultipleTransactions` | Tests multiple transaction addition |
| `ComputeMerkleRootEmptyTransactions` | Edge case: empty block |
| `ComputeMerkleRootSingleTransaction` | Single transaction merkle root |
| `ComputeMerkleRootMultipleTransactions` | Multiple transaction merkle tree |
| `ComputeMerkleRootDeterministic` | Ensures deterministic merkle roots |
| `ValidateDetectsTamperedTransaction` | Transaction tampering detection |
| `MerkleRootHexFormat` | Hex format validation |
| `TransactionOrderMatters` | Transaction order affects merkle root |

**Block Hash Tests (transferred from BlockHeader):**
| Test Name | Purpose |
|-----------|---------|
| `ComputeHashReturnsNonEmpty` | Validates hash computation produces output |
| `DifferentBlockDataProducesDifferentHash` | Confirms data changes affect hash |
| `BlockPreviousHashLinking` | Validates chain linking mechanism |
| `ModifyingHeaderFieldChangesBlockHash` | Confirms field modifications change hash |
| `AllBlockHeaderFieldsAffectHash` | Tests that each field contributes to hash |
| `MiningIncrementingNonce` | Simulates proof-of-work mining |
| `RepeatedBlockHashComputationIsDeterministic` | Confirms deterministic hashing |
| `BlockHashReturnsHexFormat` | Validates hex format output |
| `DifferentBlocksProduceDifferentLength64Hashes` | Confirms unique hashes |

---

## References

- **Google Test Documentation:** https://google.github.io/googletest/
- **Crypto++ Documentation:** https://cryptopp.com/
- **CMake Documentation:** https://cmake.org/documentation/
- **MSVC Runtime Library Options:** https://docs.microsoft.com/en-us/cpp/c-runtime-library/crt-library-features

---

## Summary

This comprehensive test suite provides thorough validation of all core blockchain components: Transaction, BlockHeader, and Block classes. The test suite ensures correctness, determinism, and tampering detection.


## Issues Encountered and Solutions

### Issue 1: Runtime Library Mismatch (LNK2038)

**Problem:**
```
error LNK2038: mismatch detected for 'RuntimeLibrary':
value 'MT_StaticRelease' doesn't match value 'MD_DynamicRelease'
```

**Root Cause:**
- Google Test was compiled with MT (static) runtime by default
- The test code defaulted to MD (dynamic) runtime
- MSVC linker rejects mixing different runtime libraries in the same executable

**Solution:**
1. Set `CMAKE_MSVC_RUNTIME_LIBRARY = "MultiThreaded"` in CMakeLists.txt
2. Added explicit compiler flags: `/MT` to all compile commands
3. Set `gtest_force_shared_crt = OFF` to force gtest to use static MT

**Implementation:**
```cmake
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /DNDEBUG")
set(gtest_force_shared_crt OFF)
```

**Why this works:**
- Forces entire project (test code + gtest + CryptoPP) to use the same MT runtime
- Eliminates runtime library conflicts
- Ensures static linking of MSVCRT

---

### Issue 2: Missing Core Dependencies

**Problem:**
- CMakeLists.txt initially didn't include `../Core/Transaction.cpp`
- Compilation failed with "undefined reference" errors

**Solution:**
- Added Transaction.cpp to the test executable sources:
```cmake
add_executable(test_Transaction
    test_Transaction.cpp
    ../Core/Transaction.cpp)  # Added this
```

---

### Issue 3: Unresolved CryptoPP Symbols

**Problem:**
```
error LNK2019: unresolved external symbol "bool __cdecl CryptoPP::SHA256::Update(...)"
[Multiple similar errors for sha256.cpp, hex.cpp, etc.]
```

**Root Cause:**
- Transaction uses CryptoPP's SHA256 hashing
- CryptoPP source files weren't being compiled into the build

**Solution:**
Rather than linking a pre-built CryptoPP library, we built CryptoPP from source:

```cmake
set(CRYPTOPP_SOURCES
    sha.cpp hex.cpp basecode.cpp algparam.cpp
    allocate.cpp filters.cpp cryptlib.cpp misc.cpp
    iterhash.cpp queue.cpp fips140.cpp mqueue.cpp)

add_library(cryptopp_lib STATIC ${CRYPTOPP_SOURCES})
target_link_libraries(test_Transaction PRIVATE cryptopp_lib)
```

**Why this approach?**
- Full CryptoPP library is massive (~100+ source files)
- Transaction only uses SHA256 hashing
- Minimal source set (12 files) includes only what's needed
- Reduces compile time and avoids unnecessary dependencies

---

### Issue 4: Assembly Function Missing (CPUID64, XGETBV64)

**Problem:**
```
error LNK2019: unresolved external symbol "void __cdecl
CryptoPP::ASM_G_GetSHA256_SIMD_FeatureMask(...)"
```

**Root Cause:**
- `cpu.cpp` references SIMD detection functions
- Assembly implementations weren't available on the platform

**Solution:**
Disabled ASM compilation in CryptoPP:
```cmake
target_compile_definitions(cryptopp_lib PUBLIC
    CRYPTOPP_DISABLE_ASM           # Disable assembly code
    CRYPTOPP_DISABLE_MIXED_MPI)    # Disable mixed precision MPI
```

**Impact:**
- No performance loss for SHA256 (C++ implementation is sufficient)
- Eliminates platform-specific assembly dependencies
- Simplifies cross-platform builds

---

### Issue 5: Missing CryptoPP Utility Function Implementations

**Problem:**
```
error LNK2019: unresolved external symbol
"bool __cdecl CryptoPP::AssignIntToInteger(...)"
error LNK2019: unresolved external symbol
"class std::basic_string<...> __cdecl CryptoPP::IntToString<unsigned __int64>(...)"
```

**Root Cause:**
- These utility functions are defined in `integer.cpp`
- Including `integer.cpp` brings in dozens of unnecessary dependencies
- The functions are only used in exception-handling paths, not in SHA256

**Solution:**
Created stub implementations in a separate source file:

```cpp
namespace CryptoPP {
  bool AssignIntToInteger(const std::type_info&, void*, const void*)
  { return false; }

  template<typename T>
  std::string IntToString(T, unsigned int)
  { return std::string(); }

  template std::string IntToString<unsigned long long>(unsigned long long, unsigned int);
}
```

**Why stubs work:**
- These functions are only called in error conditions
- Normal transaction hashing never triggers these paths
- Stubs satisfy the linker while keeping the build minimal

---

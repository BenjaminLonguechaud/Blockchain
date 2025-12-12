cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

# Force static runtime library (MT) for all targets to match Google Test
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

# Force static runtime in compiler flags as well
# The Google Test library is compiled with static MT runtime by default
set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /DNDEBUG" CACHE STRING "C++ flags" FORCE)
set(CMAKE_C_FLAGS_RELEASE "/MT /O2 /DNDEBUG" CACHE STRING "C flags" FORCE)

set(GOOGLETEST_VERSION 1.17.0)

project(BlockchainTests)

enable_testing()

# Force gtest to use static runtime matching our setting
# Google Test Integration
set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
add_subdirectory(googletest)

include_directories(googletest/include)

# Build CryptoPP as a static library
# Instead of linking a pre-built library, we compile only the minimal required sources
set(CRYPTOPP_SOURCES
    ../cryptopp/sha.cpp
    ../cryptopp/hex.cpp
    ../cryptopp/basecode.cpp
    ../cryptopp/algparam.cpp
    ../cryptopp/allocate.cpp
    ../cryptopp/filters.cpp
    ../cryptopp/cryptlib.cpp
    ../cryptopp/misc.cpp
    ../cryptopp/iterhash.cpp
    ../cryptopp/queue.cpp
    ../cryptopp/fips140.cpp
    ../cryptopp/mqueue.cpp
)

add_library(cryptopp_lib STATIC ${CRYPTOPP_SOURCES})
target_include_directories(cryptopp_lib PUBLIC ../cryptopp)
set_target_properties(cryptopp_lib PROPERTIES COMPILE_FLAGS "/MT")
# Disable ASM and some advanced features to avoid linking issues
target_compile_definitions(cryptopp_lib PUBLIC CRYPTOPP_DISABLE_ASM CRYPTOPP_DISABLE_MIXED_MPI)

### Transaction Test ###
add_executable(test_Transaction
    ../Core/Transaction.cpp
    ../Core/CoreObject.cpp
    test_Transaction.cpp
)
target_include_directories(test_Transaction PRIVATE ../Core ../cryptopp)
# Link against Google Test and CryptoPP
target_link_libraries(test_Transaction PRIVATE gtest_main gtest cryptopp_lib)
# Include stub implementations for CryptoPP utility functions
target_sources(test_Transaction PRIVATE cryptopp_stubs.cpp)
include(GoogleTest)
gtest_discover_tests(test_Transaction)

### BlockHeader Test ###
add_executable(test_BlockHeader
    ../Core/Blockheader.cpp
    ../Core/CoreObject.cpp
    test_BlockHeader.cpp
)
target_include_directories(test_BlockHeader PRIVATE ../Core ../cryptopp)
# Link against Google Test and CryptoPP
target_link_libraries(test_BlockHeader PRIVATE gtest_main gtest cryptopp_lib)
# Include stub implementations for CryptoPP utility functions
target_sources(test_BlockHeader PRIVATE cryptopp_stubs.cpp)
gtest_discover_tests(test_BlockHeader)

### Block Test ###
add_executable(test_Block
    ../Core/Block.cpp
    ../Core/Blockheader.cpp
    ../Core/Transaction.cpp
    ../Core/CoreObject.cpp
    test_Block.cpp
)
target_include_directories(test_Block PRIVATE ../Core ../cryptopp)
# Link against Google Test and CryptoPP
target_link_libraries(test_Block PRIVATE gtest_main gtest cryptopp_lib)
# Include stub implementations for CryptoPP utility functions
target_sources(test_Block PRIVATE cryptopp_stubs.cpp)
gtest_discover_tests(test_Block)

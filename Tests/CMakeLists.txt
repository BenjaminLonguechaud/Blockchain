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

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Force gtest to use static runtime matching our setting
# Google Test Integration
set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
add_subdirectory(googletest)

include_directories(googletest/include)

### Transaction Test ###
add_executable(test_Transaction
    ../Core/Transaction.cpp
    ../Core/CoreObject.cpp
    test_Transaction.cpp
)
target_include_directories(test_Transaction PRIVATE ../Core)
# Link against Google Test and OpenSSL
target_link_libraries(test_Transaction PRIVATE gtest_main gtest OpenSSL::Crypto)
include(GoogleTest)
gtest_discover_tests(test_Transaction)

### BlockHeader Test ###
add_executable(test_BlockHeader
    ../Core/Blockheader.cpp
    ../Core/CoreObject.cpp
    test_BlockHeader.cpp
)
target_include_directories(test_BlockHeader PRIVATE ../Core)
# Link against Google Test and OpenSSL
target_link_libraries(test_BlockHeader PRIVATE gtest_main gtest OpenSSL::Crypto)
gtest_discover_tests(test_BlockHeader)

### Block Test ###
add_executable(test_Block
    ../Core/Block.cpp
    ../Core/Blockheader.cpp
    ../Core/Transaction.cpp
    ../Core/CoreObject.cpp
    test_Block.cpp
)
target_include_directories(test_Block PRIVATE ../Core)
# Link against Google Test and OpenSSL
target_link_libraries(test_Block PRIVATE gtest_main gtest OpenSSL::Crypto)
gtest_discover_tests(test_Block)

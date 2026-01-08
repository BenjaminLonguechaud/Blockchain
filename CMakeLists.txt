cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(Blockchain)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Source files
set(SOURCES
    main.cpp
    Core/Block.cpp
    Core/Blockchain.cpp
    Core/Blockheader.cpp
    Core/CoreObject.cpp
    Core/Transaction.cpp
)

# Create executable
add_executable(blockchain ${SOURCES})

# Link OpenSSL
target_link_libraries(blockchain PRIVATE OpenSSL::Crypto OpenSSL::SSL)

# Include directories
target_include_directories(blockchain PRIVATE ${OPENSSL_INCLUDE_DIR})

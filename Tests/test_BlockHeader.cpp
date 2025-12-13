#include "gtest/gtest.h"
#include "Blockheader.h"
#include <chrono>

// Helper: sleep a little for timestamp differentiation
static void sleepShort() {
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
}

// ====================================================================
//  BlockHeader Construction Tests
// ====================================================================

TEST(BlockHeaderTest, DefaultConstructorInitializesFields) {
    BlockHeader header;

    EXPECT_EQ(header.version, 1);
    EXPECT_EQ(header.hashPrevBlock, "");
    EXPECT_EQ(header.hashMerkleRoot, "");
    EXPECT_EQ(header.timestamp, 0);
    EXPECT_EQ(header.nonce, 0);
    EXPECT_EQ(header.difficulty, 0);
    EXPECT_EQ(header.blockHash, "");
}

// ====================================================================
//  Serialization Tests
// ====================================================================

TEST(BlockHeaderTest, SerializeIncludesAllFields) {
    BlockHeader header;
    header.version = 1;
    header.hashPrevBlock = "aabbccdd";
    header.hashMerkleRoot = "11223344";
    header.timestamp = 1234567890;
    header.nonce = 12345;
    header.difficulty = 256;

    std::string serialized = header.serialize();

    // Verify all fields are included in serialization
    EXPECT_NE(serialized.find("1234567890"), std::string::npos);  // timestamp
    EXPECT_NE(serialized.find("1"), std::string::npos);           // version
    EXPECT_NE(serialized.find("aabbccdd"), std::string::npos);    // hashPrevBlock
    EXPECT_NE(serialized.find("11223344"), std::string::npos);    // hashMerkleRoot
    EXPECT_NE(serialized.find("12345"), std::string::npos);       // nonce
    EXPECT_NE(serialized.find("256"), std::string::npos);         // difficulty
}

TEST(BlockHeaderTest, SerializationIsConsistent) {
    BlockHeader header;
    header.version = 2;
    header.hashPrevBlock = "prev123";
    header.hashMerkleRoot = "merkle456";
    header.timestamp = 9876543210;
    header.nonce = 999;
    header.difficulty = 512;
    header.blockHash = "current789";

    std::string ser1 = header.serialize();
    std::string ser2 = header.serialize();

    EXPECT_EQ(ser1, ser2);
}

// ====================================================================
//  Hashing Tests
// ====================================================================

TEST(BlockHeaderTest, ComputeHashReturnsNonEmpty) {
    BlockHeader header;
    header.version = 1;
    header.hashPrevBlock = "genesis";
    header.hashMerkleRoot = "transactions";
    header.timestamp = 1000000;
    header.nonce = 42;
    header.difficulty = 1;

    std::string hash = header.computeHash();

    EXPECT_FALSE(hash.empty());
    EXPECT_GT(hash.length(), 0);
}

TEST(BlockHeaderTest, DifferentDataProducesDifferentHash) {
    BlockHeader header1;
    header1.version = 1;
    header1.hashPrevBlock = "block1";
    header1.hashMerkleRoot = "merkle1";
    header1.timestamp = 1000;
    header1.nonce = 10;
    header1.difficulty = 1;

    BlockHeader header2;
    header2.version = 1;
    header2.hashPrevBlock = "block1";
    header2.hashMerkleRoot = "merkle1";
    header2.timestamp = 1000;
    header2.nonce = 10;
    header2.difficulty = 2;  // Different difficulty

    std::string hash1 = header1.computeHash();
    std::string hash2 = header2.computeHash();

    EXPECT_NE(hash1, hash2);
}

// ====================================================================
//  Block Linking Tests (Chain Verification)
// ====================================================================

TEST(BlockHeaderTest, PreviousBlockHashLinking) {
    // Simulate creating two blocks in a chain
    BlockHeader block1;
    block1.version = 1;
    block1.hashPrevBlock = "genesis_hash";
    block1.hashMerkleRoot = "transactions_1";
    block1.timestamp = 1000;
    block1.nonce = 100;
    block1.difficulty = 1;

    // Compute hash of first block
    std::string block1Hash = block1.computeHash();

    // Second block references first block
    BlockHeader block2;
    block2.version = 1;
    block2.hashPrevBlock = block1Hash;  // Link to previous block
    block2.hashMerkleRoot = "transactions_2";
    block2.timestamp = 2000;
    block2.nonce = 200;
    block2.difficulty = 1;

    // Verify linking
    EXPECT_EQ(block2.hashPrevBlock, block1Hash);
    EXPECT_NE(block1Hash, block2.computeHash());
}

// ====================================================================
//  Field Modification Tests
// ====================================================================

TEST(BlockHeaderTest, ModifyingFieldChangesHash) {
    BlockHeader header;
    header.version = 1;
    header.hashPrevBlock = "parent";
    header.hashMerkleRoot = "merkle";
    header.timestamp = 3000;
    header.nonce = 0;
    header.difficulty = 128;

    std::string originalHash = header.computeHash();

    // Modify the nonce (as would happen during mining)
    header.nonce = 1;
    std::string modifiedHash = header.computeHash();

    EXPECT_NE(originalHash, modifiedHash);
}

TEST(BlockHeaderTest, AllFieldsAffectHash) {
    BlockHeader baseline;
    baseline.version = 1;
    baseline.hashPrevBlock = "prev";
    baseline.hashMerkleRoot = "merkle";
    baseline.timestamp = 1000;
    baseline.nonce = 0;
    baseline.difficulty = 1;

    std::string baselineHash = baseline.computeHash();

    // Test each field affects the hash
    BlockHeader modified = baseline;

    // Version change
    modified.version = 2;
    EXPECT_NE(baselineHash, modified.computeHash());
    modified.version = baseline.version;

    // hashPrevBlock change
    modified.hashPrevBlock = "different";
    EXPECT_NE(baselineHash, modified.computeHash());
    modified.hashPrevBlock = baseline.hashPrevBlock;

    // hashMerkleRoot change
    modified.hashMerkleRoot = "different_merkle";
    EXPECT_NE(baselineHash, modified.computeHash());
    modified.hashMerkleRoot = baseline.hashMerkleRoot;

    // timestamp change
    modified.timestamp = 2000;
    EXPECT_NE(baselineHash, modified.computeHash());
    modified.timestamp = baseline.timestamp;

    // difficulty change
    modified.difficulty = 512;
    EXPECT_NE(baselineHash, modified.computeHash());
    modified.difficulty = baseline.difficulty;

    // nonce change
    modified.nonce = 12345;
    EXPECT_NE(baselineHash, modified.computeHash());
}

// ====================================================================
//  Edge Case Tests
// ====================================================================

TEST(BlockHeaderTest, ZeroTimestampIsValid) {
    BlockHeader header;
    header.version = 1;
    header.timestamp = 0;
    header.nonce = 0;
    header.difficulty = 0;

    std::string hash = header.computeHash();

    EXPECT_FALSE(hash.empty());
}

TEST(BlockHeaderTest, LargeValueHandling) {
    BlockHeader header;
    header.version = UINT64_MAX;
    header.timestamp = UINT64_MAX;
    header.nonce = UINT32_MAX;
    header.difficulty = UINT32_MAX;

    std::string hash = header.computeHash();

    EXPECT_FALSE(hash.empty());
}

TEST(BlockHeaderTest, EmptyHashFieldsAreValid) {
    BlockHeader header;
    header.version = 1;
    header.hashPrevBlock = "";
    header.hashMerkleRoot = "";
    header.timestamp = 1000;
    header.nonce = 0;
    header.difficulty = 1;
    header.blockHash = "";

    std::string hash = header.computeHash();
    
    EXPECT_FALSE(hash.empty());
}

// ====================================================================
//  Mining Simulation Tests
// ====================================================================

TEST(BlockHeaderTest, MiningSimulationIncrementingNonce) {
    BlockHeader header;
    header.version = 1;
    header.hashPrevBlock = "genesis";
    header.hashMerkleRoot = "txs";
    header.timestamp = 1000;
    header.difficulty = 4;
    header.nonce = 0;

    // Simulate mining: increment nonce until hash meets difficulty
    std::string lastHash;
    for (uint32_t attempts = 0; attempts < 1000; ++attempts) {
        std::string hash = header.computeHash();
        lastHash = hash;

        // Check if hash starts with leading zeros (simplified difficulty)
        if (hash[0] == '0' && hash[1] == '0') {
            EXPECT_EQ(header.nonce, attempts);
            break;
        }

        header.nonce++;
    }

    EXPECT_FALSE(lastHash.empty());
}

// ====================================================================
//  Determinism Tests
// ====================================================================

TEST(BlockHeaderTest, RepeatedComputeHashesDeterministic) {
    BlockHeader header;
    header.version = 1;
    header.hashPrevBlock = "block_prev";
    header.hashMerkleRoot = "merkle_root";
    header.timestamp = 9999;
    header.nonce = 777;
    header.difficulty = 32;

    std::vector<std::string> hashes;
    for (int i = 0; i < 5; ++i) {
        hashes.push_back(header.computeHash());
    }

    // All hashes should be identical
    for (size_t i = 1; i < hashes.size(); ++i) {
        EXPECT_EQ(hashes[0], hashes[i]);
    }
}

// ====================================================================
//  Hash Format Tests
// ====================================================================

TEST(BlockHeaderTest, ComputeHashReturnsHexFormat) {
    BlockHeader header;
    header.version = 1;
    header.hashPrevBlock = "test";
    header.hashMerkleRoot = "merkle";
    header.timestamp = 1000;
    header.nonce = 0;
    header.difficulty = 1;

    std::string hash = header.computeHash();

    // SHA-256 produces 64 hex characters
    EXPECT_EQ(hash.length(), 64);

    // All characters should be valid hex
    for (char c : hash) {
        EXPECT_TRUE(std::isxdigit(c));
    }
}

TEST(BlockHeaderTest, DifferentHeadersProduceDifferentLength64Hashes) {
    BlockHeader header1;
    header1.version = 1;
    header1.hashPrevBlock = "a";
    header1.hashMerkleRoot = "b";
    header1.timestamp = 100;
    header1.nonce = 1;
    header1.difficulty = 1;

    BlockHeader header2;
    header2.version = 1;
    header2.hashPrevBlock = "x";
    header2.hashMerkleRoot = "y";
    header2.timestamp = 200;
    header2.nonce = 2;
    header2.difficulty = 2;

    std::string hash1 = header1.computeHash();
    std::string hash2 = header2.computeHash();

    EXPECT_EQ(hash1.length(), 64);
    EXPECT_EQ(hash2.length(), 64);
    EXPECT_NE(hash1, hash2);
}

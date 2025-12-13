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

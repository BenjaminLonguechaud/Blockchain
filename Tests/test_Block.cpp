#include "gtest/gtest.h"
#include "Block.h"
#include <vector>

// ====================================================================
//  Block Construction Tests
// ====================================================================

TEST(BlockTest, DefaultConstructorInitializesFields) {
    Block block;

    // Header fields should be initialized with default values
    EXPECT_EQ(block.header.version, 1);
    EXPECT_EQ(block.header.hashPrevBlock, "");
    EXPECT_EQ(block.header.hashMerkleRoot, "");
    EXPECT_EQ(block.header.timestamp, 0);
    EXPECT_EQ(block.header.nonce, 0);
    EXPECT_EQ(block.header.difficulty, 0);

    // Transactions vector should be empty
    EXPECT_TRUE(block.transactions.empty());
    EXPECT_EQ(block.transactions.size(), 0);
}

// ====================================================================
//  Transaction Addition Tests
// ====================================================================

TEST(BlockTest, CanAddTransactionToBlock) {
    Block block;

    // Create a transaction
    TxIn input("prev_txid", 0, "sig", "key");
    TxOut output(100, "addr");
    Transaction tx({input}, {output});

    // Add transaction to block
    block.transactions.push_back(tx);

    EXPECT_EQ(block.transactions.size(), 1);
    EXPECT_EQ(block.transactions[0].txid, tx.txid);
}

TEST(BlockTest, CanAddMultipleTransactions) {
    Block block;

    // Create and add multiple transactions
    for (int i = 0; i < 5; ++i) {
        TxIn input("prev_" + std::to_string(i), i, "sig_" + std::to_string(i), "key_" + std::to_string(i));
        TxOut output(100 + i, "addr_" + std::to_string(i));
        block.transactions.push_back(Transaction({input}, {output}));
    }

    EXPECT_EQ(block.transactions.size(), 5);
}

// ====================================================================
//  Merkle Root Tests
// ====================================================================

TEST(BlockTest, ComputeMerkleRootEmptyTransactions) {
    Block block;

    // Empty block should return empty merkle root
    block.computeMerkleRoot();

    EXPECT_TRUE(block.header.hashMerkleRoot.empty());
    EXPECT_EQ(block.header.hashMerkleRoot, "");
}

TEST(BlockTest, ComputeMerkleRootSingleTransaction) {
    Block block;

    // Add one transaction
    TxIn input("prev", 0, "sig", "key");
    TxOut output(100, "addr");
    Transaction tx({input}, {output});
    block.transactions.push_back(tx);

    // Merkle root of single transaction should be the transaction's TXID
    block.computeMerkleRoot();

    EXPECT_FALSE(block.header.hashMerkleRoot.empty());
    EXPECT_EQ(block.header.hashMerkleRoot, tx.txid);
}

TEST(BlockTest, ComputeMerkleRootMultipleTransactions) {
    Block block;

    // Add multiple transactions
    for (int i = 0; i < 8; ++i) {
        TxIn input("prev_" + std::to_string(i), i, "sig_" + std::to_string(i), "key_" + std::to_string(i));
        TxOut output(100 + i, "addr_" + std::to_string(i));
        block.transactions.push_back(Transaction({input}, {output}));
    }

    block.computeMerkleRoot();

    EXPECT_FALSE(block.header.hashMerkleRoot.empty());
    EXPECT_EQ(block.header.hashMerkleRoot.length(), 64);
}

TEST(BlockTest, ComputeMerkleRootDeterministic) {
    Block block1;
    Block block2;

    // Create identical transactions in both blocks
    for (int i = 0; i < 4; ++i) {
        TxIn input("prev_" + std::to_string(i), i, "sig_" + std::to_string(i), "key_" + std::to_string(i));
        TxOut output(100 + i, "addr_" + std::to_string(i));

        Transaction tx({input}, {output});
        block1.transactions.push_back(tx);

        // Create separate Transaction with same data
        Transaction tx2({input}, {output});
        block2.transactions.push_back(tx2);
    }

    block1.computeMerkleRoot();
    block2.computeMerkleRoot();

    // Same transactions should produce same merkle root
    EXPECT_EQ(block1.header.hashMerkleRoot, block2.header.hashMerkleRoot);
}

// ====================================================================
//  Block Validation Tests
// ====================================================================

TEST(BlockTest, ValidateDetectsTamperedTransaction) {
    Block block1;

    // Create a transaction and add it to block1
    TxIn input("prev", 0, "sig", "key");
    TxOut output(100, "addr");
    Transaction tx({input}, {output});
    block1.transactions.push_back(tx);

    // Compute correct merkle root for block1
    block1.computeMerkleRoot();

    // Now create a DIFFERENT transaction with modified output
    Block block2;
    TxOut tamperedOutput(200, "addr");  // Different amount!
    Transaction tamperedTx({input}, {tamperedOutput});
    block2.transactions.push_back(tamperedTx);

    // Compute merkle root for the tampered block
    block2.computeMerkleRoot();

    // The merkle roots should be different because the transactions are different
    EXPECT_FALSE(block1.header.hashMerkleRoot == block2.header.hashMerkleRoot);
}

// ====================================================================
//  Block Merkle Tree Structure Tests
// ====================================================================

TEST(BlockTest, MerkleRootHexFormat) {
    Block block;

    // Add transaction
    TxIn input("prev", 0, "sig", "key");
    TxOut output(100, "addr");
    block.transactions.push_back(Transaction({input}, {output}));

    block.computeMerkleRoot();

    // SHA-256 produces 64 hex characters
    EXPECT_EQ(block.header.hashMerkleRoot.length(), 64);

    // All characters should be valid hex
    for (char c : block.header.hashMerkleRoot) {
        EXPECT_TRUE(std::isxdigit(c));
    }
}

TEST(BlockTest, TransactionOrderMatters) {
    Block block1;
    Block block2;

    // Create two transactions
    TxIn input1("prev1", 0, "sig1", "key1");
    TxOut output1(100, "addr1");
    Transaction tx1({input1}, {output1});

    TxIn input2("prev2", 1, "sig2", "key2");
    TxOut output2(200, "addr2");
    Transaction tx2({input2}, {output2});

    // Block 1: tx1, tx2
    block1.transactions.push_back(tx1);
    block1.transactions.push_back(tx2);

    // Block 2: tx2, tx1 (reversed order)
    block2.transactions.push_back(tx2);
    block2.transactions.push_back(tx1);

    block1.computeMerkleRoot();
    block2.computeMerkleRoot();

    // Different order should produce different merkle root
    EXPECT_NE(block1.header.hashMerkleRoot, block2.header.hashMerkleRoot);
}

// ====================================================================
//  Block Hash Tests (transferred from test_BlockHeader)
// ====================================================================

TEST(BlockTest, ComputeHashReturnsNonEmpty) {
    Block block;
    block.header.version = 1;
    block.header.hashPrevBlock = "genesis";
    block.header.hashMerkleRoot = "transactions";
    block.header.timestamp = 1000000;
    block.header.nonce = 42;
    block.header.difficulty = 1;

    block.computeHash();

    EXPECT_FALSE(block.header.blockHash.empty());
    EXPECT_GT(block.header.blockHash.length(), 0);
}

TEST(BlockTest, DifferentBlockDataProducesDifferentHash) {
    Block block1;
    block1.header.version = 1;
    block1.header.hashPrevBlock = "block1";
    block1.header.hashMerkleRoot = "merkle1";
    block1.header.timestamp = 1000;
    block1.header.nonce = 10;
    block1.header.difficulty = 1;

    Block block2;
    block2.header.version = 1;
    block2.header.hashPrevBlock = "block1";
    block2.header.hashMerkleRoot = "merkle1";
    block2.header.timestamp = 1000;
    block2.header.nonce = 10;
    block2.header.difficulty = 2;  // Different difficulty

    block1.computeHash();
    block2.computeHash();

    EXPECT_NE(block1.header.blockHash, block2.header.blockHash);
}

TEST(BlockTest, BlockPreviousHashLinking) {
    // Simulate creating two blocks in a chain
    Block block1;
    block1.header.version = 1;
    block1.header.hashPrevBlock = "genesis_hash";
    block1.header.hashMerkleRoot = "transactions_1";
    block1.header.timestamp = 1000;
    block1.header.nonce = 100;
    block1.header.difficulty = 1;

    // Compute hash of first block
    block1.computeHash();
    std::string block1Hash = block1.header.blockHash;

    // Second block references first block
    Block block2;
    block2.header.version = 1;
    block2.header.hashPrevBlock = block1Hash;  // Link to previous block
    block2.header.hashMerkleRoot = "transactions_2";
    block2.header.timestamp = 2000;
    block2.header.nonce = 200;
    block2.header.difficulty = 1;

    // Verify linking
    EXPECT_EQ(block2.header.hashPrevBlock, block1Hash);
    block2.computeHash();
    EXPECT_NE(block1Hash, block2.header.blockHash);
}

TEST(BlockTest, ModifyingHeaderFieldChangesBlockHash) {
    Block block;
    block.header.version = 1;
    block.header.hashPrevBlock = "parent";
    block.header.hashMerkleRoot = "merkle";
    block.header.timestamp = 3000;
    block.header.nonce = 0;
    block.header.difficulty = 128;

    block.computeHash();
    std::string originalHash = block.header.blockHash;

    // Modify the nonce (as would happen during mining)
    block.header.nonce = 1;
    block.computeHash();
    std::string modifiedHash = block.header.blockHash;

    EXPECT_NE(originalHash, modifiedHash);
}

TEST(BlockTest, AllBlockHeaderFieldsAffectHash) {
    Block baseline;
    baseline.header.version = 1;
    baseline.header.hashPrevBlock = "prev";
    baseline.header.hashMerkleRoot = "merkle";
    baseline.header.timestamp = 1000;
    baseline.header.nonce = 0;
    baseline.header.difficulty = 1;

    baseline.computeHash();
    std::string baselineHash = baseline.header.blockHash;

    // Test each field affects the hash
    Block modified = baseline;

    // Version change
    modified.header.version = 2;
    modified.computeHash();
    EXPECT_NE(baselineHash, modified.header.blockHash);
    modified.header.version = baseline.header.version;

    // hashPrevBlock change
    modified.header.hashPrevBlock = "different";
    modified.computeHash();
    EXPECT_NE(baselineHash, modified.header.blockHash);
    modified.header.hashPrevBlock = baseline.header.hashPrevBlock;

    // hashMerkleRoot change
    modified.header.hashMerkleRoot = "different_merkle";
    modified.computeHash();
    EXPECT_NE(baselineHash, modified.header.blockHash);
    modified.header.hashMerkleRoot = baseline.header.hashMerkleRoot;

    // timestamp change
    modified.header.timestamp = 2000;
    modified.computeHash();
    EXPECT_NE(baselineHash, modified.header.blockHash);
    modified.header.timestamp = baseline.header.timestamp;

    // difficulty change
    modified.header.difficulty = 512;
    modified.computeHash();
    EXPECT_NE(baselineHash, modified.header.blockHash);
    modified.header.difficulty = baseline.header.difficulty;

    // nonce change
    modified.header.nonce = 12345;
    modified.computeHash();
    EXPECT_NE(baselineHash, modified.header.blockHash);
}

TEST(BlockTest, MiningIncrementingNonce) {
    Block block;
    block.header.version = 1;
    block.header.hashPrevBlock = "genesis";
    block.header.hashMerkleRoot = "txs";
    block.header.timestamp = 1000;
    block.header.difficulty = 4;
    block.header.nonce = 0;

    block.mineBlock();

    EXPECT_FALSE(block.header.nonce <= 1);
    EXPECT_FALSE(block.header.blockHash.empty());
}

TEST(BlockTest, RepeatedBlockHashComputationIsDeterministic) {
    Block block;
    block.header.version = 1;
    block.header.hashPrevBlock = "block_prev";
    block.header.hashMerkleRoot = "merkle_root";
    block.header.timestamp = 9999;
    block.header.nonce = 777;
    block.header.difficulty = 32;
    block.header.blockHash = "";  // Clear blockHash before first computation

    block.computeHash();
    std::string firstHash = block.header.blockHash;

    // Computing hash again without changing any fields should produce same result
    block.computeHash();
    std::string secondHash = block.header.blockHash;

    EXPECT_EQ(firstHash, secondHash);
}

TEST(BlockTest, BlockHashReturnsHexFormat) {
    Block block;
    block.header.version = 1;
    block.header.hashPrevBlock = "test";
    block.header.hashMerkleRoot = "merkle";
    block.header.timestamp = 1000;
    block.header.nonce = 0;
    block.header.difficulty = 1;

    block.computeHash();
    std::string hash = block.header.blockHash;

    // SHA-256 produces 64 hex characters
    EXPECT_EQ(hash.length(), 64);

    // All characters should be valid hex
    for (char c : hash) {
        EXPECT_TRUE(std::isxdigit(c));
    }
}

TEST(BlockTest, DifferentBlocksProduceDifferentLength64Hashes) {
    Block block1;
    block1.header.version = 1;
    block1.header.hashPrevBlock = "a";
    block1.header.hashMerkleRoot = "b";
    block1.header.timestamp = 100;
    block1.header.nonce = 1;
    block1.header.difficulty = 1;

    Block block2;
    block2.header.version = 1;
    block2.header.hashPrevBlock = "x";
    block2.header.hashMerkleRoot = "y";
    block2.header.timestamp = 200;
    block2.header.nonce = 2;
    block2.header.difficulty = 2;

    block1.computeHash();
    block2.computeHash();

    EXPECT_EQ(block1.header.blockHash.length(), 64);
    EXPECT_EQ(block2.header.blockHash.length(), 64);
    EXPECT_NE(block1.header.blockHash, block2.header.blockHash);
}

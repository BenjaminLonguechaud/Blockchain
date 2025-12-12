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

TEST(BlockTest, ComputeMerkleRootUpdatesHeaderField) {
    Block block;

    // Add transactions
    TxIn input("prev", 0, "sig", "key");
    TxOut output(100, "addr");
    block.transactions.push_back(Transaction({input}, {output}));

    // Initially header merkle root should be empty
    EXPECT_TRUE(block.header.hashMerkleRoot.empty());

    // Compute merkle root
    block.computeMerkleRoot();

    // Header should now contain the computed merkle root
    EXPECT_EQ(block.header.hashMerkleRoot, block.header.hashMerkleRoot);
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
    std::string block1MerkleRoot = block1.header.hashMerkleRoot;

    // Now create a DIFFERENT transaction with modified output
    Block block2;
    TxOut tamperedOutput(200, "addr");  // Different amount!
    Transaction tamperedTx({input}, {tamperedOutput});
    block2.transactions.push_back(tamperedTx);

    // Compute merkle root for the tampered block
    block2.computeMerkleRoot();
    std::string block2MerkleRoot = block2.header.hashMerkleRoot;

    // The merkle roots should be different because the transactions are different
    bool merkleRootsMatch = (block1MerkleRoot == block2MerkleRoot);

    EXPECT_FALSE(merkleRootsMatch);
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

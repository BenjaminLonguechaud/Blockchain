#include "gtest/gtest.h"
#include "Transaction.h"
#include <chrono>
#include <thread>

// Helper: sleep a little for timestamp differentiation
static void sleepShort() {
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
}

// ------------------------------------------------------------
//  Transaction Construction Tests
// ------------------------------------------------------------

TEST(TransactionTest, ConstructorSetsTimestamp) {
    uint64_t before = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();
    Transaction tx;
    uint64_t after  = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();

    EXPECT_GE(tx.timestamp, before);
    EXPECT_LE(tx.timestamp, after);
}

TEST(TransactionTest, ParameterizedConstructorSetsTimestamp) {
    TxIn in1("abc", 0, "sig1", "pk1");
    TxOut out1(1000, "recipientHash");

    uint64_t before = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();
    Transaction tx({in1}, {out1});
    uint64_t after = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();

    EXPECT_GE(tx.timestamp, before);
    EXPECT_LE(tx.timestamp, after);
    EXPECT_EQ(tx.inputs.size(), 1);
    EXPECT_EQ(tx.outputs.size(), 1);
}

TEST(TransactionTest, IDComputedAutomaticallyAndDeterministically) {
    TxIn in1("aaa", 0, "sigA", "pkA");
    TxOut out1(50, "alice");

    Transaction tx({in1}, {out1});
    TXID id1 = tx.txid;
    TXID id2 = tx.computeHash();

    ASSERT_FALSE(id1.empty());
    EXPECT_EQ(id1, id2);  // ID must match the hash
}

// ------------------------------------------------------------
//  Serialization Tests
// ------------------------------------------------------------

TEST(TransactionTest, SerializeIncludesTimestamp) {
    TxIn in("prev", 1, "sig", "pk");
    TxOut out(999, "bob");

    Transaction tx({in}, {out});

    std::string ser = tx.serialize();

    // Timestamp must appear as decimal text
    EXPECT_NE(ser.find(std::to_string(tx.timestamp)), std::string::npos);
}

TEST(TransactionTest, SerializeIncludesInputsAndOutputs) {
    TxIn in("prevXYZ", 3, "sigXYZ", "pkXYZ");
    TxOut out(12345, "hashXYZ");

    Transaction tx({in}, {out});

    std::string ser = tx.serialize();

    EXPECT_NE(ser.find("prevXYZ"), std::string::npos);
    EXPECT_NE(ser.find("sigXYZ"), std::string::npos);
    EXPECT_NE(ser.find("hashXYZ"), std::string::npos);
}

// ------------------------------------------------------------
//  Deterministic Hashing Tests
// ------------------------------------------------------------

TEST(TransactionTest, SameDataSameHash) {
    TxIn in("idA", 0, "sigA", "pkA");
    TxOut out(100, "destA");

    Transaction tx1({in}, {out});
    sleepShort(); // Make sure timestamp differs for tx2
    Transaction tx2({in}, {out});

    // Because timestamps differ, hashes MUST differ
    EXPECT_NE(tx1.timestamp, tx2.timestamp);
    EXPECT_NE(tx1.computeHash(), tx2.computeHash());
}

TEST(TransactionTest, IdenticalTimestampYieldsIdenticalHashes) {
    TxIn in("prev", 0, "sig", "pk");
    TxOut out(11, "bob");

    Transaction tx1({in}, {out});
    Transaction tx2({in}, {out});

    // Force tx2 timestamp to match tx1
    tx2.timestamp = tx1.timestamp;

    EXPECT_EQ(tx1.serialize(), tx2.serialize());
    EXPECT_EQ(tx1.computeHash(), tx2.computeHash());
}

// ------------------------------------------------------------
//  Error Handling / Edge Cases
// ------------------------------------------------------------

TEST(TransactionTest, EmptyInputsOrOutputsAllowedButHashValid) {
    Transaction tx({}, {});
    EXPECT_FALSE(tx.computeHash().empty());
}

TEST(TransactionTest, MultipleInputsOutputsSerializeCorrectly) {
    std::vector<TxIn> ins = {
        TxIn("A", 1, "sigA", "pkA"),
        TxIn("B", 2, "sigB", "pkB")
    };

    std::vector<TxOut> outs = {
        TxOut(10, "alice"),
        TxOut(20, "bob")
    };

    Transaction tx(ins, outs);
    std::string ser = tx.serialize();

    EXPECT_NE(ser.find("A"), std::string::npos);
    EXPECT_NE(ser.find("B"), std::string::npos);
    EXPECT_NE(ser.find("alice"), std::string::npos);
    EXPECT_NE(ser.find("bob"), std::string::npos);
}

// ------------------------------------------------------------
//  ID Stability Test
// ------------------------------------------------------------

TEST(TransactionTest, IDStableAfterInitialComputation) {
    TxIn in("aaa", 5, "sig", "pk");
    TxOut out(77, "dest");

    Transaction tx({in}, {out});
    TXID idFirst = tx.txid;
    TXID idSecond = tx.computeHash();

    EXPECT_EQ(idFirst, idSecond); // ID stored must match computed hash
}

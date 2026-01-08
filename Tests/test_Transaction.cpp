#include "gtest/gtest.h"
#include "Transaction.h"
#include <chrono>
#include <thread>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>

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
    tx.computeHash();

    ASSERT_FALSE(id1.empty());
    EXPECT_EQ(id1, tx.txid);  // ID must match the hash
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

    tx1.computeHash();
    tx2.computeHash();

    // Because timestamps differ, hashes MUST differ
    EXPECT_NE(tx1.timestamp, tx2.timestamp);
    EXPECT_NE(tx1.txid, tx2.txid);
}

TEST(TransactionTest, IdenticalTimestampYieldsIdenticalHashes) {
    TxIn in("prev", 0, "sig", "pk");
    TxOut out(11, "bob");

    Transaction tx1({in}, {out});
    Transaction tx2({in}, {out});

    // Force tx2 timestamp to match tx1
    tx2.timestamp = tx1.timestamp;

    tx1.computeHash();
    tx2.computeHash();

    EXPECT_EQ(tx1.serialize(), tx2.serialize());
    EXPECT_EQ(tx1.txid, tx2.txid);
}

// ------------------------------------------------------------
//  Error Handling / Edge Cases
// ------------------------------------------------------------

TEST(TransactionTest, EmptyInputsOrOutputsAllowedButHashValid) {
    Transaction tx({}, {});
    tx.computeHash();
    EXPECT_FALSE(tx.txid.empty());
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
    tx.computeHash();
    TXID idSecond = tx.txid;

    EXPECT_EQ(idFirst, idSecond); // ID stored must match computed hash
}
// ------------------------------------------------------------
//  Signing Tests
// ------------------------------------------------------------

// Helper function to generate an RSA key pair for testing
EVP_PKEY* GenerateTestKeyPair() {
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);

    if (!ctx) {
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        ERR_print_errors_fp(stderr);
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) {
        ERR_print_errors_fp(stderr);
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        ERR_print_errors_fp(stderr);
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    EVP_PKEY_CTX_free(ctx);
    return pkey;
}

TEST(TransactionTest, SignatureGeneratedSuccessfully) {
    TxIn in("prev", 0, "sig", "pk");
    TxOut out(100, "alice");

    Transaction tx({in}, {out});
    EVP_PKEY *pkey = GenerateTestKeyPair();
    ASSERT_NE(pkey, nullptr) << "Failed to generate test key pair";

    // Before signing, signature should be empty
    EXPECT_EQ(tx.txsignature, "");

    tx.sign(pkey);

    // After signing, signature should not be empty
    EXPECT_NE(tx.txsignature, "");

    EVP_PKEY_free(pkey);
}

TEST(TransactionTest, DifferentTransactionsDifferentSignatures) {
    TxIn in1("txA", 0, "sig", "pk");
    TxOut out1(100, "alice");

    TxIn in2("txB", 0, "sig", "pk");
    TxOut out2(100, "alice");

    Transaction tx1({in1}, {out1});
    Transaction tx2({in2}, {out2});

    EVP_PKEY *pkey = GenerateTestKeyPair();
    ASSERT_NE(pkey, nullptr);

    tx1.sign(pkey);
    tx2.sign(pkey);

    // Different transactions should produce different signatures
    // (with extremely high probability for RSA signatures)
    EXPECT_NE(tx1.txsignature, tx2.txsignature);

    EVP_PKEY_free(pkey);
}

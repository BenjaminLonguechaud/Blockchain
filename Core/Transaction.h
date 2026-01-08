#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "CoreObject.h"
#include <vector>
#include <cstdint>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <openssl/err.h>

/**
 * @file Transaction.h
 * @brief Definition of the Transaction class representing a blockchain transaction.
 * @details This class encapsulates the essential components of a blockchain transaction,
 *         including inputs, outputs, timestamp, and methods for serialization,
 *        hash computation, and validation.
 * https://en.bitcoin.it/wiki/Transaction
 */

// Input of a transaction to ensure ownership and prevent double spending.
struct TxIn {
    std::string prevTxID;           // Pointer to a previous transaction that created the output being spent.
    uint32_t outputIndex;           // Index of the output of the to-be-used transaction
    std::string signature;          // Signature proving ownership of the referenced output.
    std::string publicKey;          // Public key used to verify signature

    TxIn(const std::string& prevTxID,
        uint32_t outputIndex,
        const std::string& signature,
        const std::string& pubKey)
        : prevTxID(prevTxID),
        outputIndex(outputIndex),
        signature(signature),
        publicKey(publicKey) {}
};

// Output of a transaction. Makes the output spendable only by the owner of the corresponding private key.
struct TxOut {
    // The value locked in this output (e.g., coins).
    uint64_t amount;
    // Hash of the recipient's public key; Identifies the owner.
    // This makes the output spendable only by the owner of the corresponding private key.
    std::string publicKeyHash;

TxOut(uint64_t amount, const std::string& publicKeyHash)
        : amount(amount), publicKeyHash(publicKeyHash) {}
};

class Transaction : public CoreObject {
public:

    std::string txid;
    std::string txsignature;
    std::vector<TxIn> inputs;
    std::vector<TxOut> outputs;
    uint64_t timestamp;

    Transaction();

    Transaction(std::string id,
                    std::vector<TxIn> in,
                    std::vector<TxOut> out,
                    uint64_t ts) :
            txid(std::move(id)),
            inputs(std::move(in)),
            outputs(std::move(out)),
            timestamp(ts) {}

    Transaction(const std::vector<TxIn>& ins,
                    const std::vector<TxOut>& outs);

    /**
     * Computes the hash of the Transaction.
     */
    void computeHash();

    // Sign the transaction
    void sign(EVP_PKEY *pkey);

    // Validate transaction structure
    bool validate() const;

    // Serialize the transaction into a deterministic string
    std::string serialize() const override;

};

#endif // TRANSACTION_H

#include <string>
#include <vector>
#include <cstdint>

/**
 * Represents a value transfer or message.
 */

// 256-bit hash represented as an hex string
using TXID = std::string;

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

class Transaction {
public:

    std::string txid;
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

    // Compute unique TXID (SHA-256 of the serialized content)
    TXID computeHash() const;

    // Validate transaction structur
    bool validate() const;

    // Convert the whole transaction into a deterministic string
    std::string serialize() const;
};

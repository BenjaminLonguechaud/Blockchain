#include <Transaction.h>
#include <sstream>
#include <iomanip>
#include <chrono>


Transaction::Transaction()
    : timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count()),
    txsignature("")
{
    computeHash();
}

Transaction::Transaction(const std::vector<TxIn>& ins,
                         const std::vector<TxOut>& outs)
    : inputs(ins), outputs(outs),
      timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count()),
    txsignature("")
{
    computeHash();
}

// -----------------------------------------------------------------------------
//  Checks structural correctness.
// -----------------------------------------------------------------------------
bool Transaction::validate() const
{
    // Basic checks
    if (inputs.empty() || outputs.empty()) {
        return false; // A valid transaction must have at least one input and one output
    }

    for (const auto& output : outputs) {
        if (output.amount == 0) {
            return false; // Outputs must have a non-zero amount
        }
    }

    // Additional checks can be added here (e.g., signature verification)

    return true; // Passed all checks
}

// -----------------------------------------------------------------------------
//  computeHash()
//  Produces the TXID by hashing the serialized transaction.
// -----------------------------------------------------------------------------
void Transaction::computeHash()
{
    // Converts a Transaction object into a single, deterministic
    // sequence of bytes (or a string) that can be hashed
    std::string data = serialize();
    CryptoPP::SHA256 hash;
    std::string digest;
    CryptoPP::StringSource(data, true,
        new CryptoPP::HashFilter(hash,
            new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(digest)
            )
        )
    );

    txid = digest;
}

// -----------------------------------------------------------------------------
//  serialize()
//  Produces a canonical string describing the transaction.
// -----------------------------------------------------------------------------
std::string Transaction::serialize() const
{
    std::ostringstream oss;
    oss << timestamp;

    for (const auto& input : inputs) {
        oss << input.prevTxID << input.outputIndex
            << input.signature << input.publicKey;
    }

    for (const auto& output : outputs) {
        oss << output.amount << output.publicKeyHash;
    }

    return oss.str();
}

void Transaction::sign()
{
    // Placeholder for signing logic
    // In a real implementation, this would involve cryptographic operations
    // to sign the transaction inputs using the private keys corresponding
    // to the public keys in the inputs.

    // For now, we just set a dummy signature for each input
    for (auto& input : inputs) {
        input.signature = "dummy_signature";
    }
}

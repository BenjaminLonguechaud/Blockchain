#include "Block.h"
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

// -----------------------------------------------------------------------------
//  computeHash()
// -----------------------------------------------------------------------------
void Block::computeHash()
{
    // Converts a Transaction object into a single, deterministic
    // sequence of bytes (or a string) that can be hashed
    std::string data = serialize();
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);

    // Convert the hash to a hex string

    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    _header.blockHash = oss.str();
}

// -----------------------------------------------------------------------------
// mineBlock()
// Repeatedly changes the nonce and recalculates the hash until it meets
// the difficulty target.
// -----------------------------------------------------------------------------
void Block::mine()
{
    std::string target(_header.difficulty / 4, '0'); // Each hex digit represents 4 bits

    do {
        _header.nonce++;
        computeHash();
    } while (_header.blockHash.substr(0, target.size()) != target);
}

// -----------------------------------------------------------------------------
// computeMerkleRoot()
// Computes the Merkle root from the transactions in the block.
// -----------------------------------------------------------------------------
void Block::computeMerkleRoot()
{
    // Step 1: Validate input - Check if the block contains any transactions
    // If empty, do nothing since there's no data to hash
    if (_transactions.empty()) {
        return;
    }

    // Step 2: Build the initial Merkle tree level (leaves)
    // Collect all transaction IDs (TXIDs) which will be the leaves of the Merkle tree
    // Each TXID is already a SHA-256 hash of the transaction data
    std::vector<std::string> merkleLeaves;
    for (const auto& tx : _transactions) {
        merkleLeaves.push_back(tx.txid);
    }

    // Step 3: Iteratively build the Merkle tree from bottom to top
    // Continue until only one hash remains (the Merkle root)
    // Each iteration processes pairs of hashes from the current level
    while (merkleLeaves.size() > 1) {
        std::vector<std::string> newLevel;

        // Step 3a: Process pairs of hashes at the current tree level
        // Iterate through leaves in pairs (i, i+1)
        for (size_t i = 0; i < merkleLeaves.size(); i += 2) {

            // Step 3b: Check if we have a complete pair of hashes
            if (i + 1 < merkleLeaves.size()) {
                // Step 3b-i: Concatenate the two sibling hashes
                // In Bitcoin, this would be concatenated and hashed once
                // Some implementations use double SHA-256 (SHA256(SHA256(data)))
                std::string combined = merkleLeaves[i] + merkleLeaves[i + 1];

                // Step 3b-ii: Compute SHA-256 hash of the concatenated pair
                unsigned char hash[SHA256_DIGEST_LENGTH];
                SHA256(reinterpret_cast<const unsigned char*>(combined.c_str()), combined.size(), hash);
                // Convert the hash to a hex string
                std::ostringstream oss;
                for (int j = 0; j < SHA256_DIGEST_LENGTH; ++j) {
                    oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[j]);
                }
                // Step 3b-iii: Add the resulting parent hash to the next level
                newLevel.push_back(oss.str());
            } else {
                // Step 3c: Handle odd number of leaves at current level
                // If there's an odd leaf remaining with no pair,
                // it is promoted to the next level unchanged
                // (Alternative implementations might hash it with itself)
                newLevel.push_back(merkleLeaves[i]);
            }
        }

        // Step 3d: Move to the next tree level (one level closer to the root)
        // Replace current level with newly computed parent hashes
        merkleLeaves = newLevel;
    }

    // Step 4: Store the root hash
    // After all iterations, merkleLeaves contains exactly one element: the Merkle root
    // This root is a deterministic hash of all transactions in the block
    _header.hashMerkleRoot = merkleLeaves.front();
}

std::string Block::serialize() const
{
    std::ostringstream oss;

    // Serialize header information
    oss << _header.version;
    oss << _header.hashPrevBlock;
    oss << _header.hashMerkleRoot;
    oss << _header.timestamp;
    oss << _header.nonce;
    oss << _header.difficulty;

    // Serialize all transactions
    for (const auto& tx : _transactions) {
        oss << tx.serialize();
    }

    return oss.str();
}

const std::string& Block::getHash() const
{
    return _header.blockHash;
}

const std::string& Block::getPreviousHash() const
{
    return _prevBlockHash;
}

const std::string& Block::getMerkleRoot() const
{
    return _header.hashMerkleRoot;
}

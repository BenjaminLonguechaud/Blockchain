#include <Block.h>
#include <sstream>

// -----------------------------------------------------------------------------
//  computeHash()
// -----------------------------------------------------------------------------
void Block::computeHash()
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

    header.blockHash = digest;
}

// -----------------------------------------------------------------------------
// mineBlock()
// Repeatedly changes the nonce and recalculates the hash until it meets
// the difficulty target.
// -----------------------------------------------------------------------------
void Block::mineBlock()
{
    std::string target(header.difficulty / 4, '0'); // Each hex digit represents 4 bits

    do {
        header.nonce++;
        computeHash();
    } while (header.blockHash.substr(0, target.size()) != target);
}

// -----------------------------------------------------------------------------
// computeMerkleRoot()
// Computes the Merkle root from the transactions in the block.
// -----------------------------------------------------------------------------
void Block::computeMerkleRoot()
{
    // Step 1: Validate input - Check if the block contains any transactions
    // If empty, do nothing since there's no data to hash
    if (transactions.empty()) {
        return;
    }

    // Step 2: Build the initial Merkle tree level (leaves)
    // Collect all transaction IDs (TXIDs) which will be the leaves of the Merkle tree
    // Each TXID is already a SHA-256 hash of the transaction data
    std::vector<std::string> merkleLeaves;
    for (const auto& tx : transactions) {
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
                CryptoPP::SHA256 hash;
                std::string digest;
                CryptoPP::StringSource(combined, true,
                    new CryptoPP::HashFilter(hash,
                        new CryptoPP::HexEncoder(
                            new CryptoPP::StringSink(digest)
                        )
                    )
                );

                // Step 3b-iii: Add the resulting parent hash to the next level
                newLevel.push_back(digest);
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
    header.hashMerkleRoot = merkleLeaves.front();
}

std::string Block::serialize() const
{
    std::ostringstream oss;

    // Serialize header information
    oss << header.version;
    oss << header.hashPrevBlock;
    oss << header.hashMerkleRoot;
    oss << header.timestamp;
    oss << header.nonce;
    oss << header.difficulty;

    // Serialize all transactions
    for (const auto& tx : transactions) {
        oss << tx.serialize();
    }

    return oss.str();
}

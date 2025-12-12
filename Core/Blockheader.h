#ifndef BLOCKHEADER_H
#define BLOCKHEADER_H

#include <CoreObject.h>
#include <string>

/**
 * @file blockheader.h
 * @brief Definition of the BlockHeader class representing a blockchain block header.
 * We use a similar approach as Bitcoin's block header structure.
 * https://en.bitcoin.it/wiki/Block_hashing_algorithm
 * @details This class encapsulates the essential metadata for a blockchain block,
 *          including version, previous block hash, Merkle root, timestamp, difficulty target,
 *		  	and nonce. It provides methods for serialization, hash computation, and validation.
 * 			A block header in a blockchain is a concise summary (metadata) of a block, used for
 * 			linking blocks, securing the chain, and efficient verification.
 *
 * For Proof-of-Work (Mining), Miners hash the header (including the nonce) repeatedly until they
 * find a hash below a target difficulty, solving the puzzle and adding the block.
 */

class BlockHeader : public CoreObject {
public:

    // Block version number
    uint64_t version;
    // 256-bit hash of the previous block header, forming the chain.
    std::string hashPrevBlock;
    // 256-bit hash based on all of the transactions in the block
    std::string hashMerkleRoot;
    // Current block timestamp as seconds since 1970-01-01T00:00 UTC
    uint64_t timestamp;
    // 32-bit number (starts at 0 and is incremented for each hash) used in the proof-of-work algorithm
    uint32_t nonce;
    // Difficulty is a measure of how difficult it is to find a hash below a given target.
    uint32_t difficulty;
    //The hash of the block itself, serving as the block's unique identifier and proof of work.
    std::string blockHash;

    // Constructor to initialize BlockHeader with default values
    BlockHeader();
    BlockHeader(uint64_t ver,
                    const std::string& prevHash,
                    const std::string& merkleRoot,
                    uint64_t time,
					uint32_t nonce,
                    uint32_t diff)
	    : version(ver),
          hashPrevBlock(prevHash),
          hashMerkleRoot(merkleRoot),
          timestamp(time),
          nonce(nonce),
          difficulty(diff),
          blockHash("") {}

    // Serialize the transaction into a deterministic string
    std::string serialize() const override;

};

#endif // BLOCKHEADER_H

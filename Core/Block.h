#ifndef BLOCK_H
#define BLOCK_H

#include <Blockheader.h>
#include <Transaction.h>

/**
 * @file Block.h
 * @brief Definition of the Block class representing a blockchain block.
 * @details This class encapsulates the essential components of a blockchain block,
 *         including the block header and a list of transactions.
 * https://en.bitcoin.it/wiki/Block
 */
class Block : public CoreObject {
public:

    Block(): header(), transactions() {}

    /**
     * Mining Method (PoW), repeatedly changes the nonce and recalculates
     * the hash until it meets the difficulty target.
     */
    void computeMerkleRoot();

    /**
     * Computes the hash of the block.
     */
    void Block::computeHash();

    /**
     * Mines the block by finding a nonce that results in a hash
     * below the target defined by the difficulty.
     */
    void mineBlock();

    /**
     * Serializes the block into a deterministic string representation.
     * Combines header and all transaction data for hashing.
     */
    std::string serialize() const override;

    BlockHeader header;
    std::vector<Transaction> transactions;
};

#endif // BLOCK_H

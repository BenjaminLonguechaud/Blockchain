#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "Block.h"
#include <vector>

/**
 * @file Blockchain.h
 * @brief Definition of the Blockchain class representing a chain of blocks.
 * @details This class encapsulates the essential structure of a blockchain,
 *          which is a linked list of blocks, each containing a set of transactions.
 */
class Blockchain : public CoreObject {
public:

    /**
     * Default constructor
     */
    Blockchain();

    /**
     * Adds a new block to the blockchain after validation.
     */
    bool addBlock(const Block& newBlock);

    /**
     * Creates a new block from a pool of transactions.
     */
    Block createBlock(const std::vector<Transaction>& pool);

    /**
     * Validates the entire blockchain for integrity.
    */
    bool validateChain() const;

    /**
     * Serializes the block into a deterministic string representation.
     * Combines header and all transaction data for hashing.
     */
    std::string serialize() const override;

    void print() const;

private:
    // The first block of a block chain
    // https://en.bitcoin.it/wiki/Genesis_block
    Block createGenesisBlock();

    std::vector<Block> _chain;

};

#endif // BLOCKCHAIN_H

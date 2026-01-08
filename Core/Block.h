#ifndef BLOCK_H
#define BLOCK_H

#include "Blockheader.h"
#include "Transaction.h"

/**
 * @file Block.h
 * @brief Definition of the Block class representing a blockchain block.
 * @details This class encapsulates the essential components of a blockchain block,
 *         including the block header and a list of transactions.
 * https://en.bitcoin.it/wiki/Block
 */
class Block : public CoreObject {
public:

    Block(): _header(), _transactions() {}

    Block(const std::vector<Transaction>& transactions): _header(),
    _transactions(transactions) {}

    /**
     * Mining Method (PoW), repeatedly changes the nonce and recalculates
     * the hash until it meets the difficulty target.
     */
    void computeMerkleRoot();

    /**
     * Computes the hash of the block.
     */
    void computeHash();

    /**
     * Mines the block by finding a nonce that results in a hash
     * below the target defined by the difficulty.
     */
    void mine();

    /**
     * Serializes the block into a deterministic string representation.
     * Combines header and all transaction data for hashing.
     */
    std::string serialize() const override;

    /**
     * Accessor to block hash.
     */
    const std::string& getHash() const;

    /**
     * Accessor to previous block hash.
     */
    const std::string& getPreviousHash() const;

    const BlockHeader getHeader() const {return _header;};
    void setHeader(const BlockHeader& header) {_header = header;};

    const std::string& Block::getMerkleRoot() const;

private:
    BlockHeader _header;
    std::vector<Transaction> _transactions;
    std::string _prevBlockHash;
};

#endif // BLOCK_H

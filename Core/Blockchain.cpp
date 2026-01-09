#include "Blockchain.h"
#include <sstream>
#include <iostream>
#include <chrono>

std::string Blockchain::serialize() const
{
    std::ostringstream oss;
    for (const auto& block : _chain) {
        oss << block.serialize();
    }
    return oss.str();
}

Block Blockchain::createGenesisBlock()
{
    Block genesisBlock("0");
    // The genesis block is almost always hardcoded into the software of the
    // applications that utilize its block chain.
    uint64_t version = 1;
    std::string prevHash = "0"; // No previous block
    std::string hashMerkleRoot = "0x4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b";
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
    uint32_t nonce = 0;
    uint32_t difficulty = 0x3;

    genesisBlock.setHeader(BlockHeader(version, prevHash, hashMerkleRoot,
                                      timestamp, nonce, difficulty));
    genesisBlock.mine();
    genesisBlock.computeMerkleRoot();
    return genesisBlock;
}

Blockchain::Blockchain()
{
    _chain.push_back(createGenesisBlock());
}

bool Blockchain::addBlock(const Block& newBlock)
{
    // Check previous hash
    const std::string& lastHash = _chain.empty() ? "" : _chain.back().getHash();
    if (!_chain.empty() && newBlock.getPreviousHash() != lastHash) {
        std::cerr << "Error: previous hash does not match chain tip\n";
        return false;
    }

    // TODO: Validate Merkle root and proof of work

    _chain.push_back(newBlock);
    return true;
}

bool Blockchain::validateChain() const
{
    // TODO

    return true;
}

void Blockchain::print() const
{
    std::cout << "=== Blockchain (" << _chain.size() << " blocks) ===\n";

    for (const auto& block : _chain) {
        std::cout << "---------------------------------------\n";
        block.getHeader().print();
    }
}

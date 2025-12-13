#include <Blockchain.h>

std::string Blockchain::serialize() const
{
    std::ostringstream oss;
    for (const auto& block : chain) {
        oss << block.serialize();
    }
    return oss.str();
}

Block Blockchain::createGenesisBlock()
{
    Block genesisBlock;
    // The genesis block is almost always hardcoded into the software of the
    // applications that utilize its block chain.
    uint64_t version = 1;
    std::string prevHash = "0"; // No previous block
    std::string hashMerkleRoot = "0x4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b";
    uint64_t timestamp = 1231006505;
    uint32_t nonce = 2083236893;
    uint32_t difficulty = 0x1d00ffff;

    genesisBlock.header = BlockHeader(version, prevHash, hashMerkleRoot,
                                      timestamp, nonce, difficulty);
    return genesisBlock;
}

Blockchain::Blockchain()
{
    chain.push_back(createGenesisBlock());
}


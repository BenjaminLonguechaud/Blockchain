#include "Blockheader.h"
#include <sstream>
#include <iostream>

// -----------------------------------------------------------------------------
//  BlockHeader Constructor with Default Values
// -----------------------------------------------------------------------------
BlockHeader::BlockHeader()
    : version(1),
      hashPrevBlock(""),
      hashMerkleRoot(""),
      timestamp(0),
      nonce(0),
      difficulty(0),
      blockHash("")
{
}

// -----------------------------------------------------------------------------
//  serialize()
//  Produces a canonical string describing the transaction.
// -----------------------------------------------------------------------------
std::string BlockHeader::serialize() const
{
    std::ostringstream oss;
    oss << timestamp;
    oss << version;
    oss << hashPrevBlock;
    oss << hashMerkleRoot;
    oss << nonce;
    oss << difficulty;

    return oss.str();
}

// -----------------------------------------------------------------------------
//  print()
//  Prints all BlockHeader parameters to std::cout
// -----------------------------------------------------------------------------
void BlockHeader::print() const
{
    std::cout << "BlockHeader Information:" << std::endl;
    std::cout << "  Version: " << version << std::endl;
    std::cout << "  Previous Block Hash: " << hashPrevBlock << std::endl;
    std::cout << "  Merkle Root Hash: " << hashMerkleRoot << std::endl;
    std::cout << "  Timestamp: " << timestamp << std::endl;
    std::cout << "  Nonce: " << nonce << std::endl;
    std::cout << "  Difficulty: " << difficulty << std::endl;
    std::cout << "  Block Hash: " << blockHash << std::endl;
}

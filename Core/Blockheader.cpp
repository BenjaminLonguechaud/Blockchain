#include "Blockheader.h"
#include <sstream>

// -----------------------------------------------------------------------------
//  computeHash()
//  Produces the block hash by hashing the serialized block header.
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

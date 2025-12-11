#include <CoreObject.h>

// -----------------------------------------------------------------------------
//  computeHash()
//  Produces the TXID by hashing the serialized transaction.
// -----------------------------------------------------------------------------
TXID CoreObject::computeHash() const
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

    return digest;
}

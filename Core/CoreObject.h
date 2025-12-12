#ifndef COREOBJECT_H
#define COREOBJECT_H

#include <string>
#include <../cryptopp/sha.h>
#include <../cryptopp/hex.h>

// 256-bit hash represented as an hex string
using TXID = std::string;

/**
 * @file CoreObject.h
 * @brief Definition of the CoreObject class as a base for core blockchain entities.
 */

class CoreObject {
public:
	// Convert the whole transaction into a deterministic string
    virtual std::string serialize() const = 0;

    virtual ~CoreObject() = default;
};

#endif // COREOBJECT_H

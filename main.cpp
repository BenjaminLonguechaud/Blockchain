#include "Core/Blockchain.h"
#include "Core/Transaction.h"
#include "Core/Block.h"
#include <iostream>
#include <openssl/evp.h>
#include <openssl/encoder.h>

EVP_PKEY* generateKeyPair() {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    if (!ctx) return nullptr;

    if (EVP_PKEY_keygen_init(ctx) <= 0) return nullptr;
    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_secp256k1) <= 0)
        return nullptr;

    EVP_PKEY* pkey = nullptr;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) return nullptr;

    EVP_PKEY_CTX_free(ctx);
    return pkey;
}

std::string getPublicKey(EVP_PKEY* pkey) {
    OSSL_ENCODER_CTX* ctx = OSSL_ENCODER_CTX_new_for_pkey(
        pkey,
        OSSL_KEYMGMT_SELECT_PUBLIC_KEY,
        "DER",
        nullptr,
        nullptr
    );

    if (!ctx) return {};

    unsigned char* data = nullptr;
    size_t len = 0;

    if (!OSSL_ENCODER_to_data(ctx, &data, &len)) {
        OSSL_ENCODER_CTX_free(ctx);
        return {};
    }

    std::vector<unsigned char> pubKey(data, data + len);

    OPENSSL_free(data);
    OSSL_ENCODER_CTX_free(ctx);

    return std::string(reinterpret_cast< char const* >(pubKey.data()), pubKey.size());
}

std::string hashPublicKey(
    const std::string& pubKey) {

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, pubKey.data(), pubKey.size());
    SHA256_Final(hash, &sha256);

    return std::string(reinterpret_cast< char const* >(hash));
}

int main() {

    std::cout << "Blockchain Implementation Demo" << std::endl;
    std::cout << "==============================" << std::endl << std::endl;

    // Generate keys
	std::cout << "[1] Generate key pairs and hash the receiver public key..." << std::endl;
    EVP_PKEY* senderKey = generateKeyPair();
    EVP_PKEY* receiverKey = generateKeyPair();
    auto senderPubKey = getPublicKey(senderKey);
    auto receiverPubKey = getPublicKey(receiverKey);
    auto receiverHash = hashPublicKey(receiverPubKey);

    // Create a blockchain instance
	std::cout << "[2] Creating blockchain with genesis block and default complexity..." << std::endl;
    Blockchain blockchain;

    // Create the first transaction (TxIn and TxOut)
    std::cout << "[3] Creating transaction..." << std::endl;
    // Signature left as a random string for simplicity
    TxIn input("0000000000000000000000000000000000000000000000000000000000000000", 0, "sig1", senderPubKey);
    // Amount set to a random value for demonstration
    TxOut output(50, receiverHash);

    Transaction tx({input}, {output});
    std::cout << "    Transaction 1 TXID: " << tx.txid << std::endl;
    std::cout << "    Transaction 1 Timestamp: " << tx.timestamp << " ms" << std::endl << std::endl;

    // Create Block with this transaction
    std::cout << "[4] Creating block..." << std::endl;
    std::vector<Transaction> transactions = { tx };
    Block block(transactions, blockchain.getLatestBlock().getHash());

    // Block mining
    std::cout << "[5] Mining block..." << std::endl;
    block.mine();
    std::cout << "[6] Block mined: " << block.getHash() << "\n";

    // Compute Merkle root
    std::cout << "[7] Compute Merkle root: " << block.getMerkleRoot() << "\n";
    block.computeMerkleRoot();

    std::cout << "[8] Add Block to Blockchain: " << block.getMerkleRoot() << "\n";
    blockchain.addBlock(block);

    // Validate blockchain
    std::cout << "[9] Validating block..." << std::endl;
    bool isValid = blockchain.validateChain();
    std::cout << "    Block validation result: " << (isValid ? "VALID" : "INVALID") << std::endl << std::endl;

    // 10. Print chain
    blockchain.print();

    // Cleanup
    EVP_PKEY_free(senderKey);
    EVP_PKEY_free(receiverKey);

    return 0;
}

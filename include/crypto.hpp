#ifndef _CRYPTO_HPP_
#define _CRYPTO_HPP_

#define CRT_KEY_LEN 	CryptoPP::AES::DEFAULT_KEYLENGTH
#define CRT_BLOCK_SIZE 	CryptoPP::AES::BLOCKSIZE

#include "cryptopp/aes.h"
#include "cryptopp/modes.h"
#include "cryptopp/filters.h"

using std::string;

/**
 * Provides data encryption/decryption facilities
 */
class Crypto {
public:
    /**
     * Constructor
     *
     * @param password Password for data encryption/decryption
     */
    Crypto(const string& password);

    /**
     * Decrypt a string
     *
     * @param str String to be decrypted 
     * @return Decrypted string
     */
    string decryptString(const string& str);

    /**
     * Encrypt a string
     *
     * @param str String to be encrypted
     * @return Encrypted string
     */
    string encryptString(const string& str);

private:
    byte password[CRT_KEY_LEN];
    byte iv[CryptoPP::AES::BLOCKSIZE];
};

#endif // CRYPTO

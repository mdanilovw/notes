/**
 * Implementation of class Crypto
 */

#include "crypto.hpp"

Crypto::Crypto(const string &passwdStr) {
    memset(password, 0x00, CRT_KEY_LEN);

	auto passwdLimit = passwdStr.length() < CRT_KEY_LEN? passwdStr.length(): CRT_KEY_LEN;
    auto password_c_str = passwdStr.c_str();	
    for(auto i = 0; i < passwdLimit; ++i)
        password[i] = password_c_str[i];

    memset(iv, 0x00, CRT_BLOCK_SIZE); 
}

string Crypto::encryptString(const string &str) {
	string ciphertext;

	CryptoPP::AES::Encryption aesEncryption(password, CRT_KEY_LEN);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

	CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(ciphertext));
    	stfEncryptor.Put( reinterpret_cast<const unsigned char*>(str.c_str()), str.length() + 1);
	stfEncryptor.MessageEnd();

	return ciphertext;
}

string Crypto::decryptString(const string &str) {
	string decryptedtext;

	CryptoPP::AES::Decryption aesDecryption(password, CRT_KEY_LEN);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);

	CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedtext));
    	stfDecryptor.Put(reinterpret_cast<const unsigned char*>(str.c_str()), str.size());

	stfDecryptor.MessageEnd();

	return decryptedtext;
}

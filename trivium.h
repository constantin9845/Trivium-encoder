// Author: Konstantin Yakovlev A.
// Date: 08/29/2024

#include <iostream>
#include <vector>
#include <bitset>
#include <string>
#include <cmath>

class Trivium{
	public:
		// struct that holds result of de/encryption
		// contains: data, key and IV used
		struct EncryptionResult {
	        std::bitset<80> key;
	        std::bitset<80> iv;
	        std::string encodedText;
    	};
		// generate 80 bit key/IV
		static std::bitset<80> generateKeyIV();

		// encrypt with no key and IV
		static EncryptionResult encrypt(const std::string& input);

		// encript with custom key and no IV
		static EncryptionResult encrypt(const std::string& input, const std::bitset<80>& KEY);

		// encript with custom key and IV
		static EncryptionResult encrypt(const std::string& input, const std::bitset<80>& KEY, const std::bitset<80>& IV);

		// Decrypt encoded data
		// requires same Key and IV that was used during encryption
		static EncryptionResult decrypt(const std::string& input, const std::bitset<80>& KEY, const std::bitset<80>& IV);

	private:
		// execute system specific command for number generation
		static std::string exec(const char* command);
		
		// initialize registers
		static void initPhase(std::vector<bool>& A, std::vector<bool>& B, std::vector<bool>& C, const std::bitset<80>& KEY, const std::bitset<80>& IV);
		// Warm up cipher without recording output
		static void warmUpCipher(std::vector<bool>& A, std::vector<bool>& B, std::vector<bool>& C);

		// record output from cipher
		// calculate output of each register to use as input next round
		// shift register by 1
		// returns output
		static bool clock(std::vector<bool>& A, std::vector<bool>& B, std::vector<bool>& C);
		
		// convert binary number in string format to boolean vector format
		// modifies passed vector
		static void stringToBits(std::string input, std::vector<bool>& X);

		// convert a bitset to string
		// creates a new bitset
		// returns bitset 
		static std::bitset<80> stringToBits(std::string input);

		// convert boolean vector to string format
		// returns string
		static std::string bitsToString(std::vector<bool>& bits);

		// en/decrypts data by calling clock function on each character in X vector
		// stores result in Y vector
		static void encode(std::vector<bool>& X, std::vector<bool>& Y, std::vector<bool>& A, std::vector<bool>& B, std::vector<bool>& C);
};

#include <iostream>
#include <vector>
#include <bitset>
#include <string>
#include <cmath>

class Trivium{
	public:
		// result struct 
		struct EncryptionResult {
	        std::bitset<80> key;
	        std::bitset<80> iv;
	        std::string encodedText;
    	};

		static std::bitset<80> generateKeyIV();

		// encrypt with no key and IV
		static EncryptionResult encrypt(const std::string& input);

		// encript with custom key and no IV
		static EncryptionResult encrypt(const std::string& input, const std::bitset<80>& KEY);

		// encript with custom key and IV
		static EncryptionResult encrypt(const std::string& input, const std::bitset<80>& KEY, const std::bitset<80>& IV);


		static EncryptionResult decrypt(const std::string& input, const std::bitset<80>& KEY, const std::bitset<80>& IV);

	private:

		static std::string exec(const char* command);

		static void initPhase(std::vector<bool>& A, std::vector<bool>& B, std::vector<bool>& C, const std::bitset<80>& KEY, const std::bitset<80>& IV);
		static void warmUpCipher(std::vector<bool>& A, std::vector<bool>& B, std::vector<bool>& C);
		static bool clock(std::vector<bool>& A, std::vector<bool>& B, std::vector<bool>& C);

		static void stringToBits(std::string input, std::vector<bool>& X);
		static std::bitset<80> stringToBits(std::string input);
		static std::string bitsToString(std::vector<bool>& bits);

		static void encode(std::vector<bool>& X, std::vector<bool>& Y, std::vector<bool>& A, std::vector<bool>& B, std::vector<bool>& C);
};
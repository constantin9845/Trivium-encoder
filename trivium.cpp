// Author: Konstantin Yakovlev A.
// Date: 08/29/2024

#include "trivium.h"


void Trivium::stringToBits(std::string input, std::vector<bool>& X){

	// each character to bit representation
	int index = 0;
	for(char e : input){
		std::bitset<8> b(e);

		
		for(int i = 0, j = 7; i < 8; i++, j--){
			X[index + i] = b[j]; 
		}
		index+=8;
	}
}

std::bitset<80> Trivium::generateKeyIV(){

	unsigned char buffer[10];

	// check OS of user
#ifdef _WIN32
	// NEED TESTING
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(0, 255);

	for(int i = 0; i < sizeof(buffer); i++){
		buffer[i] = static_cast<unsigned char>(distr(eng));
	}

#else
	// MAC / LINUX
	std::ifstream urandom("/dev/urandom", std::ios::binary);

	if (!urandom) {
        throw std::runtime_error("Failed to open /dev/urandom");
    }

	urandom.read(reinterpret_cast<char*>(buffer), sizeof(buffer));

	if (!urandom) {
        throw std::runtime_error("Error reading from /dev/urandom");
    }

#endif

	std::bitset<80> bits;

	// Fill the bitset with the read random bytes
    for (size_t i = 0; i < sizeof(buffer); ++i) {
        for (int bit = 0; bit < 8; ++bit) {
            bits[i * 8 + bit] = (buffer[i] >> (7 - bit)) & 1;
        }
    }

    return bits;
}



std::bitset<80> Trivium::stringToBits(std::string input){

	std::bitset<80> tempBits;

	// each character to bit representation
	int index = 0;
	for(char e : input){
		std::bitset<8> b(e);

		for(int i = 0, j = 7; i < 8; i++, j--){
			tempBits[index + i] = b[j];
		}
		index+=8;
	}

	return tempBits;
}

std::string Trivium::bitsToString(std::vector<bool>& bits){
	std::string res = "";

	for(int i = 0; i < bits.size(); i+=8){

		int power = 7;
		int decimal = 0;
		for(int j = i; j < i+8; j++){
			if(bits[j]){
				decimal += pow(2,power);
			}
			power--;
		}
		res+=char(decimal);
	}
	return res;
}

void Trivium::initPhase(std::vector<bool>& A, std::vector<bool>& B, std::vector<bool>& C, const std::bitset<80>& KEY, const std::bitset<80>& IV){
	// load IV into A
	for(int i = IV.size()-1, index = 0; i >= 0; i--, index++){
		A[index] = IV[i];
	}

	// load key into B
	for(int i = KEY.size()-1, index = 0; i >= 0; i--, index++){
		B[index] = KEY[i];
	}

	// set C109,110,111 to 1
	C[108] = 1;
	C[109] = 1;
	C[110] = 1;
}

void Trivium::warmUpCipher(std::vector<bool>& A, std::vector<bool>& B, std::vector<bool>& C){
	for(int i = 0; i < 4*288; i++){
		clock(A,B,C);
	}
}

bool Trivium::clock(std::vector<bool>& A, std::vector<bool>& B, std::vector<bool>& C){
	// get output of each register
	bool outputA = A[65] ^ A[92] ^ (A[90]&A[91]);
	bool outputB = B[68] ^ B[83] ^ (B[81]&B[82]);
	bool outputC = C[65] ^ C[110] ^ (C[108]&C[109]);

	// final key bit
	bool keybit = outputA^outputB^outputC;

	// get feedback bits for next round
	bool inputA = A[68]^outputC;
	bool inputB = B[77]^outputA;
	bool inputC = C[86]^outputB;

	// shift bits of registers to right
	// right most bit lost
	// left most bit takes input calculated above
	for(int i = A.size()-1; i >= 0; i--){
		A[i] = A[i-1];
	}
	A[0] = inputA;

	for(int i = B.size()-1; i >= 0; i--){
		B[i] = B[i-1];
	}
	B[0] = inputB;

	for(int i = C.size()-1; i >= 0; i--){
		C[i] = C[i-1];
	}
	C[0] = inputC;

	return keybit;
}

void Trivium::encode(std::vector<bool>& X, std::vector<bool>& Y, std::vector<bool>& A, std::vector<bool>& B, std::vector<bool>& C){
	for(int i = 0; i < X.size(); i++){
		bool keyBit = clock(A,B,C);
		Y[i] = X[i]^keyBit;
	}
}


Trivium::Output Trivium::encrypt(const std::string& input){
	std::vector<bool> A(93);
	std::vector<bool> B(84);
	std::vector<bool> C(111);

	std::vector<bool> X(input.size()*8);
	std::vector<bool> Y(input.size()*8);

	std::bitset<80> KEY = generateKeyIV();
	std::bitset<80> IV = generateKeyIV();

	initPhase(A, B, C, KEY, IV);
	warmUpCipher(A, B, C);
	stringToBits(input, X);
	encode(X,Y,A,B,C);	

	std::string encodedString = bitsToString(Y);

	Output result;
	result.key = KEY;
	result.iv = IV;
	result.content = encodedString;

	return result;
}

Trivium::Output Trivium::encrypt(const std::string& input, const std::bitset<80>& KEY){
	std::vector<bool> A(93);
	std::vector<bool> B(84);
	std::vector<bool> C(111);

	std::vector<bool> X(input.size()*8);
	std::vector<bool> Y(input.size()*8);

	std::bitset<80> IV = generateKeyIV();

	initPhase(A, B, C, KEY, IV);
	warmUpCipher(A, B, C);
	stringToBits(input, X);
	encode(X,Y,A,B,C);	

	std::string encodedString = bitsToString(Y);

	Output result;
	result.key = KEY;
	result.iv = IV;
	result.content = encodedString;

	return result;
}

Trivium::Output Trivium::encrypt(const std::string& input, const std::bitset<80>& KEY, const std::bitset<80>& IV){
	std::vector<bool> A(93);
	std::vector<bool> B(84);
	std::vector<bool> C(111);

	std::vector<bool> X(input.size()*8);
	std::vector<bool> Y(input.size()*8);

	initPhase(A, B, C, KEY, IV);
	warmUpCipher(A, B, C);
	stringToBits(input, X);
	encode(X,Y,A,B,C);	

	std::string encodedString = bitsToString(Y);

	Output result;
	result.key = KEY;
	result.iv = IV;
	result.content = encodedString;

	return result;
}

Trivium::Output Trivium::decrypt(const std::string& input, const std::bitset<80>& KEY, const std::bitset<80>& IV){
	std::vector<bool> A(93);
	std::vector<bool> B(84);
	std::vector<bool> C(111);

	std::vector<bool> X(input.size()*8);
	std::vector<bool> Y(input.size()*8);

	initPhase(A, B, C, KEY, IV);
	warmUpCipher(A, B, C);
	stringToBits(input, X);
	encode(X,Y,A,B,C);	

	std::string decodedString = bitsToString(Y);

	Output result;
	result.key = KEY;
	result.iv = IV;
	result.content = decodedString;

	return result;
}

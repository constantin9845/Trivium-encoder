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

	for(int i = 0; i < 10; i++){
		buffer[i] = static_cast<unsigned char>(distr(eng));
	}

#else
	// MAC / LINUX
	std::ifstream urandom("/dev/urandom", std::ios::binary);

	if (!urandom) {
        throw std::runtime_error("Failed to open /dev/urandom");
    }

	urandom.read(reinterpret_cast<char*>(buffer), 10);

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

unsigned char* Trivium::generateKeyIV(int){

	unsigned char* buffer = new unsigned char[10];

	// check OS of user
#ifdef _WIN32
	// NEED TESTING
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(0, 255);

	for(int i = 0; i < 10; i++){
		buffer[i] = static_cast<unsigned char>(distr(eng));
	}

#else
	// MAC / LINUX
	std::ifstream urandom("/dev/urandom", std::ios::binary);

	if (!urandom) {
		delete[] buffer;
        throw std::runtime_error("Failed to open /dev/urandom");
    }

	urandom.read(reinterpret_cast<char*>(buffer), 10);

	if (!urandom) {
		delete[] buffer;
        throw std::runtime_error("Error reading from /dev/urandom");
    }

	urandom.close();

#endif

    return buffer;
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

void Trivium::initPhase(std::bitset<93>& A, std::bitset<84>& B, std::bitset<111>& C, const std::bitset<80>& KEY, const std::bitset<80>& IV){
	// load IV into A
	for(int i = IV.size()-1; i >= 0; i--){
		A[i] = IV[i];
	}

	// load key into B
	for(int i = KEY.size()-1; i >= 0; i--){
		B[i] = KEY[i];
	}

	// set C109,110,111 to 1
	C.flip(108);
	C.flip(109);
	C.flip(110);
}

void Trivium::initPhase(unsigned char* A, unsigned char* B, unsigned char* C, unsigned char* KEY, unsigned char* IV){
	
	// Load A and B with IV and KEY
	for(int i = 0; i < 10; i++){
		A[i] = IV[i];
		B[i] = KEY[i];
	}

	// set C109,110,111 to 1
	C[13] = C[13] | 0x07;

	// 80 - 88 - 96 - 104 - 112
	// 104 - 105 - 106 - 107 - 108 - 109 - 110 - 111
	// 0     0     0     0     0     1     1     1
}

void Trivium::warmUpCipher(std::bitset<93>& A, std::bitset<84>& B, std::bitset<111>& C){
	for(int i = 0; i < 4*288; i++){
		clock(A,B,C);
	}
}

void Trivium::warmUpCipher(unsigned char* A, unsigned char* B, unsigned char* C){
	for(int i = 0; i < 4*288; i++){
		clock(A,B,C);
	}
}

bool Trivium::clock(std::bitset<93>& A, std::bitset<84>& B, std::bitset<111>& C){
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
	A <<= 1;
	A[0] = inputA;

	B <<= 1;
	B[0] = inputB;

	C <<= 1;
	C[0] = inputC;

	return keybit;
}

bool Trivium::clock(unsigned char* A, unsigned char* B, unsigned char* C){

	// get output of each register

	// A[8][1] ^ A[11][4] ^ A[11][2]&A[11][3]
	// B[8][4] ^ B[10][3] ^ B[10][1]&B[10][2]
	// C[8][1] ^ C[13][6] ^ C[13][4]&C[13][5]

	// register outputs - //  128 64 32 16  8 4 2 1
	unsigned char outputA = ((A[8] & 0x40)>>6) ^ ((A[11] & 0x08)>>3) ^ (((A[11] & 0x20)>>5) & (A[11] & 0x10)>>4);
	unsigned char outputB = ((B[8] & 0x08)>>3) ^ ((B[10] & 0x10)>>4) ^ (((B[10] & 0x40)>>6) & (B[10] & 0x20)>>5);
	unsigned char outputC = ((C[8] & 0x40)>>6) ^ ((C[13] & 0x02)>>1) ^ (((C[13] & 0x08)>>3) & (C[13] & 0x04)>>2);

	// final key bit
	bool keyBit = (outputA^outputB^outputC) & 0x01;


	// get feedback bits for next round
	bool inputA = (((A[8] & 0x08)>>3) ^ outputC) & 0x01;
	bool inputB = (((B[9] & 0x04)>>2) ^ outputA) & 0x01;
	bool inputC = (((C[10] & 0x02)>>1) ^ outputB) & 0x01;


	// shift bits of registers to right
	// right most bit lost
	// left most bit takes input calculated above

	bool carry = false;
	// A
	for(int i = 11; i>=0; i--){

		A[i] = A[i]>>1;

		if(i > 0){
			carry = A[i-1] & 0x01;
			if(carry){
				A[i] = A[i] | 0x80;
			}
			
		}

		if(i == 0){
			if(inputA){
				A[i] = A[i] | 0x80;
			}
		}
	}

	// B
	for(int i = 10; i>=0; i--){

		B[i] = B[i]>>1;

		if(i > 0){
			carry = B[i-1] & 0x01;
			if(carry){
				B[i] = B[i] | 0x80;
			}
			
		}

		if(i == 0){
			if(inputB){
				B[i] = B[i] | 0x80;
			}
		}
	}

	// C
	for(int i = 13; i>=0; i--){

		C[i] = C[i]>>1;

		if(i > 0){
			carry = C[i-1] & 0x01;
			if(carry){
				C[i] = C[i] | 0x80;
			}
			
		}

		if(i == 0){
			if(inputC){
				C[i] = C[i] | 0x80;
			}
		}
	}

	return keyBit;
}


void Trivium::encode(std::vector<bool>& X, std::vector<bool>& Y, std::bitset<93>& A, std::bitset<84>& B, std::bitset<111>& C){
	for(int i = 0; i < X.size(); i++){
		bool keyBit = clock(A,B,C);
		Y[i] = X[i]^keyBit;
	}
}

void Trivium::encode(unsigned char* input, const int& size, unsigned char* A, unsigned char* B, unsigned char* C){

	for (int i = 0; i < size; i++) {
		unsigned char bitMask = 0x80;  
		
		for (int j = 0; j < 8; j++) {  
			bool keyBit = clock(A, B, C); 
	
			if (keyBit) {
				input[i] = input[i] ^ bitMask; 
			}
	
			bitMask >>= 1; 
		}
	}
}


// TEXT FUNCTIONS	

Trivium::Output Trivium::encrypt(const std::string& input){

	std::bitset<93> A;
	std::bitset<84> B;
	std::bitset<111> C;

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
	std::bitset<93> A;
	std::bitset<84> B;
	std::bitset<111> C;

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
	std::bitset<93> A;
	std::bitset<84> B;
	std::bitset<111> C;

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
	std::bitset<93> A;
	std::bitset<84> B;
	std::bitset<111> C;

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


// RAW BYTE FUNCTIONS

Trivium::BitOutput Trivium::encrypt(unsigned char* input, const int& size){

	unsigned char* A = new unsigned char[12];
	unsigned char* B = new unsigned char[11];
	unsigned char* C = new unsigned char[14];

	unsigned char* KEY = generateKeyIV(1);
	unsigned char* IV = generateKeyIV(1);

	initPhase(A, B, C, KEY, IV);
	warmUpCipher(A, B, C);
	encode(input, size,A,B,C);	

	BitOutput result;
	result.key = KEY;
	result.iv = IV;
	result.content = input;

	delete[] A;
	delete[] B;
	delete[] C;
	A = B = C = nullptr;	

	KEY = nullptr;
	IV = nullptr;

	return result;
}

Trivium::BitOutput Trivium::encrypt(unsigned char* input, const int& size, unsigned char* key){

	unsigned char* A = new unsigned char[12];
	unsigned char* B = new unsigned char[11];
	unsigned char* C = new unsigned char[14];

	unsigned char* IV = generateKeyIV(1);

	initPhase(A, B, C, key, IV);
	warmUpCipher(A, B, C);
	encode(input, size,A,B,C);	

	BitOutput result;
	result.key = nullptr;
	result.iv = IV;
	result.content = input;

	delete[] A;
	delete[] B;
	delete[] C;
	A = B = C = nullptr;	

	IV = nullptr;

	return result;
}

Trivium::BitOutput Trivium::encrypt(unsigned char* input, const int& size, unsigned char* key, unsigned char* iv){

	unsigned char* A = new unsigned char[12];
	unsigned char* B = new unsigned char[11];
	unsigned char* C = new unsigned char[14];

	initPhase(A, B, C, key, iv);
	warmUpCipher(A, B, C);
	encode(input, size,A,B,C);	

	BitOutput result;
	result.key = nullptr;
	result.iv = nullptr;
	result.content = input;

	delete[] A;
	delete[] B;
	delete[] C;
	A = B = C = nullptr;	

	return result;
}

Trivium::BitOutput Trivium::decrypt(unsigned char* input, const int& size, unsigned char* key, unsigned char* iv){

	unsigned char* A = new unsigned char[12];
	unsigned char* B = new unsigned char[11];
	unsigned char* C = new unsigned char[14];

	initPhase(A, B, C, key, iv);
	warmUpCipher(A, B, C);
	encode(input, size,A,B,C);	

	BitOutput result;
	result.key = nullptr;
	result.iv = nullptr;
	result.content = input;

	delete[] A;
	delete[] B;
	delete[] C;
	A = B = C = nullptr;	

	return result;
}

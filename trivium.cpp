#include <iostream>
#include <vector>
#include <bitset>
#include <string>
#include <cmath>

const std::bitset<80> IV(std::string("10100010110001111000011100100010111111101110100110001101101001000111011101111110"));

const std::bitset<80> KEY(std::string("00101000100011111111011001011101110001000010101110010010111110010110000011000111"));

// initialize cipher, add KEY and IV randomizer to register A and B
void initPhase(std::vector<bool>& A, std::vector<bool>& B, std::vector<bool>& C){

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

// perform single shift
// returns key bit
bool clock(std::vector<bool>& A, std::vector<bool>& B, std::vector<bool>& C){

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

// warm up phase
void warmUpCipher(std::vector<bool>& A, std::vector<bool>& B, std::vector<bool>& C){
	for(int i = 0; i < 4*288; i++){
		clock(A,B,C);
	}
}

// text data to bit format
void stringToBits(std::string input, std::vector<bool>& X){

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

std::string bitsToString(std::vector<bool>& Y){

	std::string res = "";

	for(int i = 0; i < Y.size(); i+=8){

		int power = 7;
		int decimal = 0;
		for(int j = i; j < i+8; j++){
			if(Y[j]){
				decimal += pow(2,power);
			}
			power--;
		}
		res+=char(decimal);
	}
	return res;
}

void encode(std::vector<bool>& X, std::vector<bool>& Y, std::vector<bool>& A, std::vector<bool>& B, std::vector<bool>& C){
	for(int i = 0; i < X.size(); i++){
		bool keyBit = clock(A,B,C);
		Y[i] = X[i]^keyBit;
	}
}

// encrypt function
std::string encrypt(std::string input){

	std::vector<bool> A(93);
	std::vector<bool> B(84);
	std::vector<bool> C(111);

	std::vector<bool> X(input.size()*8);
	std::vector<bool> Y(input.size()*8);

	initPhase(A, B, C);
	warmUpCipher(A, B, C);
	stringToBits(input, X);
	encode(X,Y,A,B,C);	
	std::string encodedText = bitsToString(Y);

	return encodedText;
}



int main(){
	std::string input = "This is a test string.";

	std::cout<<std::endl;
	std::cout<<"Initial text: "<<input<<std::endl;
	
	std::string encodedText = encrypt(input);

	std::cout<<std::endl;
	std::cout<<"Encrypted: "<<encodedText<<std::endl;

	std::string decodedText = encrypt(encodedText);

	std::cout<<std::endl;
	std::cout<<"Decrypted: "<<decodedText<<std::endl;
}



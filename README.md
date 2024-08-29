# Trivium Stream Cipher

A c++ library to encrypt text data using the trivium stream cipher.

## Usage

The library has three available functions for the user

```java
// generate a key and/or IV necessary for the ecryption function
std::bitset<80> key = Trivium::generateKeyIV();
```

```java
// Encrypt Text data.
// The function returns a struct that contains: Encoded data, the key and IV that was used
std::string text = "Hello, World!";
    
Trivium::EncryptionResult result = Trivium::encrypt(text);

std::cout << "Key: " << result.key << std::endl;
std::cout << "IV: " << result.iv << std::endl;
std::cout << "Encoded Text: " << result.encodedText << std::endl;
```

```java
// Decrypt data.
// Function requires the key and IV that was used to encrypt the data
// The function returns a struct that contains: Decoded data, the key and IV that was used

Trivium::EncryptionResult decryption = Trivium::decrypt(result.encodedText, KEY, IV);

std::cout << "Key: " << decryption.key << std::endl;
std::cout << "IV: " << decryption.iv << std::endl;
std::cout << "Decoded Text: " << decryption.encodedText << std::endl;
```

## Additional guidelines

It is important to retain the key and IV after encryption. If one of these is lost,
no decryption is possible.

Each new encryption round, a new IV is necessary. 
The same key can be reused as long as a new IV was added.

## Example

```java
#include "trivium.h"

int main(){
	std::string text = "Hello, World!";
    
 // Using the Trivium class
 Trivium::EncryptionResult result = Trivium::encrypt(text);
 
 std::cout << "Key: " << result.key << std::endl;
 std::cout << "IV: " << result.iv << std::endl;
 std::cout << "Encoded Text: " << result.encodedText << std::endl;


 Trivium::EncryptionResult decryption = Trivium::decrypt(result.encodedText, result.key, result.iv);

 std::cout << "Key: " << decryption.key << std::endl;
 std::cout << "IV: " << decryption.iv << std::endl;
 std::cout << "Decoded Text: " << decryption.encodedText << std::endl;

	return 0;
}
```

## Info

Windows Key/IV generator not tested yet.

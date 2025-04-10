# Trivium Stream Cipher

A C++ library for encrypting and decrypting text/raw byte data using the **Trivium Stream Cipher** with a cryptographically secure key generator.

## Overview

The **Trivium Stream Cipher** is a cryptographic algorithm that generates a pseudorandom bit stream used for encrypting data.

This C++ implementation of the Trivium cipher provides easy-to-use functions for encryption and decryption of text/byte data, with support for generating random keys and IVs, or using user-specified ones. The cipher operates at a high speed and is suitable for stream encryption in scenarios where lightweight, fast encryption is required.

The library allows:
- **Random key and IV generation** for encryption,
- **Encryption** of text/byte data using Trivium with customizable key and IV, and
- **Decryption** of previously encrypted data.

## Functions Overview:

The library provides the following functions for encryption and decryption:

1. **`generateKeyIV()`**  
   Generates a random **80-bit key and IV** stored in a bitset. It is used to initialize the Trivium cipher.

2. **`generateKeyIV(int)`**  
   Generates a random **80-bit key and IV** stored in unsiged char array. It is used to initialize the Trivium cipher.

3. **`encrypt(const std::string& text)`**  
   Encrypts a given **text** string using a randomly generated key and IV.

4. **`encrypt(const std::string& text, const std::bitset<80>& key)`**  
   Encrypts the **text** string using a user-provided **key** and a randomly generated IV.

5. **`encrypt(const std::string& text, const std::bitset<80>& key, const std::bitset<80>& iv)`**  
   Encrypts the **text** string using a user-provided **key** and **IV**.

6. **`encrypt(unsigned char* input, const int& size)`**  
   Encrypts a given **byte** array using a randomly generated key and IV.

7. **`encrypt(unsigned char* input, const int& size, unsigned char* key)`**  
   Encrypts the **byte** aarray using a user-provided **key** and a randomly generated IV.

8. **`encrypt(unsigned char* input, const int& size, unsigned char* key, unsigned char* iv)`**  
   Encrypts the **byte** array using a user-provided **key** and **IV**.

9. **`decrypt(const std::string& content, const std::bitset<80>& key, const std::bitset<80>& iv)`**  
   Decrypts the **content** using the provided **key** and **IV**.

10. **`decrypt(unsigned char* input, unsigned char* key, unsigned char* iv, const int& size)`**  
   Decrypts the **byte** array using the provided **key** and **IV**.


## Return type

The encrypt/decrypt functions return a Trivium object that contains the output as well as the key/iv **if these were generated during run**.

```cpp

// Text data 
Trivium::Output enc = Trivium::encrypt(input);
std::cout<<enc.content;
std::cout<<enc.key;      
std::cout<<enc.iv;       

Trivium::Output enc = Trivium::encrypt(input, KEY);
std::cout<<enc.content;
std::cout<<enc.key;      
std::cout<<enc.iv;       

Trivium::Output enc = Trivium::encrypt(input, key, IV);
std::cout<<enc.content;
std::cout<<enc.key;      
std::cout<<enc.iv;       

Trivium::Output dec = Trivium::decrypt(input, key, IV);
std::cout<<dec.content;
std::cout<<enc.key;      
std::cout<<enc.iv;       

// byte data
Trivium::BitOutput enc = Trivium::encrypt(input, 16);
std::cout<<enc.content;
std::cout<<enc.key;      
std::cout<<enc.iv;     

Trivium::BitOutput enc = Trivium::encrypt(input, 16, key);
std::cout<<enc.content;
std::cout<<enc.key;      // NULLPTR
std::cout<<enc.iv;  

Trivium::BitOutput enc = Trivium::encrypt(input, 16, key, iv);
std::cout<<enc.content;
std::cout<<enc.key;      // NULLPTR
std::cout<<enc.iv;       // NULLPTR

Trivium::BitOutput enc = Trivium::decrypt(input, 16, key, iv);
std::cout<<enc.content;
std::cout<<enc.key;      // NULLPTR
std::cout<<enc.iv;       // NULLPTR 
```

## Text Data Example

```cpp
#include "trivium.h"

int main(){
    std::string text = "Hello, World!";

    std::bitset<80> key = Trivium::generateKeyIV();
    std::bitset<80> iv = Trivium::generateKeyIV();

    // Encrypt with random key and IV
    Trivium::Output result1 = Trivium::encrypt(text);

    // Encrypt with user-defined key and random IV
    Trivium::Output result2 = Trivium::encrypt(text, key);

    // Encrypt with user-defined key and IV
    Trivium::Output result3 = Trivium::encrypt(text, key, iv);

    // Output the encrypted text, key, and IV
    std::cout << "Key: " << result1.key << std::endl;
    std::cout << "IV: " << result1.iv << std::endl;
    std::cout << "Encoded Text: " << result1.content << std::endl;

    // Decrypt the content using the same key and IV
    Trivium::Output decryption = Trivium::decrypt(result1.content, result1.key, result1.iv);

    // Output the decrypted text
    std::cout << "Key: " << decryption.key << std::endl;
    std::cout << "IV: " << decryption.iv << std::endl;
    std::cout << "Decoded Text: " << decryption.content << std::endl;

    return 0;
}
```

## Byte Data Example

```cpp
#include "trivium.h"

int main(){
    unsigned char* input = new unsigned char[]{
		0x1f, 0xd4, 0xee, 0x65,
		0x60, 0x3e, 0x61, 0x30,
		0xcf, 0xc2, 0xa8, 0x2a,
		0xb3, 0xd5, 0x6c, 0x24
	};

    unsigned char* key = Trivium::generateKeyIV(1);
    unsigned char* iv = Trivium::generateKeyIV(1);

    // Encrypt with random key and IV
    Trivium::BitOutput result1 = Trivium::encrypt(input, 16);

    // Encrypt with user-defined key and random IV
    Trivium::BitOutput result2 = Trivium::encrypt(input, 16, key);

    // Encrypt with user-defined key and IV
    Trivium::BitOutput result3 = Trivium::encrypt(input, 16, key, iv);

    // Output the encrypted text, key, and IV
    std::cout << "Key: " << result1.key << std::endl;
    for(int i = 0; i < 10; i++){
        std::cout<<std::hex<<static_cast<int>(result1.iv[i])<<" ";
    }
    std::cout << "IV: " << result1.iv << std::endl;
    for(int i = 0; i < 10; i++){
        std::cout<<std::hex<<static_cast<int>(result1.key[i])<<" ";
    }
    std::cout << "Encoded Text:" <<std::endl;
    for(int i = 0; i < 16; i++){
        std::cout<<std::hex<<static_cast<int>(result1.content[i])<<" ";
    }

    // Decrypt the content using the same key and IV
    Trivium::BitOutput decryption = Trivium::decrypt(result1.content, result1.key, result1.iv);

    // Output the decrypted text
    std::cout << "Decoded Text:" <<std::endl;
    for(int i = 0; i < 16; i++){
        std::cout<<std::hex<<static_cast<int>(decryption.content[i])<<" ";
    }

    return 0;
}
```


## Additional guidelines

It is important to retain the key and IV after encryption. Decryption is not possible if lost.

Each new encryption round, a new IV should be used. 
The same key can be reused as long as a new IV was added.

## Info

Windows Key/IV generator not tested yet.

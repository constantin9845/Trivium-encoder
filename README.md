# Trivium Stream Cipher

A C++ library for encrypting and decrypting text data using the **Trivium Stream Cipher** with a cryptographically secure key generator.

## Overview

The **Trivium Stream Cipher** is a cryptographic algorithm that generates a pseudorandom bit stream used for encrypting plaintext data.

This C++ implementation of the Trivium cipher provides easy-to-use functions for encryption and decryption of text data, with support for generating random keys and IVs, or using user-specified ones. The cipher operates at a high speed and is suitable for stream encryption in scenarios where lightweight, fast encryption is required.

The library allows:
- **Random key and IV generation** for encryption,
- **Encryption** of text data using Trivium with customizable key and IV, and
- **Decryption** of previously encrypted data.

## Functions Overview:

The library provides the following functions for encryption and decryption:

1. **`generateKeyIV()`**  
   Generates a random **80-bit key and IV**. It is used to initialize the Trivium cipher.

2. **`encrypt(const std::string& text)`**  
   Encrypts a given **text** string using a randomly generated key and IV.

3. **`encrypt(const std::string& text, const std::bitset<80>& key)`**  
   Encrypts the **text** string using a user-provided **key** and a randomly generated IV.

4. **`encrypt(const std::string& text, const std::bitset<80>& key, const std::bitset<80>& iv)`**  
   Encrypts the **text** string using a user-provided **key** and **IV**.

5. **`decrypt(const std::string& content, const std::bitset<80>& key, const std::bitset<80>& iv)`**  
   Decrypts the **content** using the provided **key** and **IV**.


## Example

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

## Additional guidelines

It is important to retain the key and IV after encryption. Decryption is not possible if lost.

Each new encryption round, a new IV should be used. 
The same key can be reused as long as a new IV was added.

## Info

Windows Key/IV generator not tested yet.

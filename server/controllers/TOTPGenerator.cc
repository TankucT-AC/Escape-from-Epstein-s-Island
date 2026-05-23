// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include <string>
#include <vector>
#include <random>
#include <iomanip>
#include <array>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdexcept>
#include "TOTPGenerator.h"

const long long DIGITS_POWER[] = {
    1LL, 10LL, 100LL, 1000LL, 10000LL, 100000LL, 
    1000000LL, 10000000LL, 100000000LL, 1000000000LL
};

unsigned char hexCharToByte(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    throw std::invalid_argument("Invalid hex character");
}

std::vector<unsigned char> hexStr2BytesFast(const std::string& hex) 
{
    std::vector<unsigned char> result;
    result.reserve(hex.length() / 2);
    
    for (size_t i = 0; i < hex.length(); i += 2) {
        unsigned char high = hexCharToByte(hex[i]);
        unsigned char low = hexCharToByte(hex[i + 1]);
        result.push_back((high << 4) | low);
    }
    
    return result;
}

const EVP_MD* TOTPGenerator::getHashAlgorithm(const std::string& crypto) 
{
    if (crypto == "HmacSHA1") return EVP_sha1();
    if (crypto == "HmacSHA256") return EVP_sha256();
    if (crypto == "HmacSHA512") return EVP_sha512();
    throw std::invalid_argument("Unsupported algorithm: " + crypto);
}

std::string TOTPGenerator::generate(const std::string& key,
                                    std::string time,
                                    const std::string& returnDigits,
                                    const std::string& crypto) 
{    
    return TOTPGenerator::generate(TOTPGenerator::base32Decode(key), time, returnDigits, crypto);
}

std::string TOTPGenerator::generate(const std::vector<unsigned char>& key_bytes,
                                    std::string time,
                                    const std::string& returnDigits,
                                    const std::string& crypto) 
{    
    int codeDigits = std::stoi(returnDigits);
    if (codeDigits <= 0 || codeDigits > 9) 
    {
        throw std::invalid_argument("codeDigits must be between 1 and 9");
    }
    
    if (time.length() < 16) 
    {
        time = std::string(16 - time.length(), '0') + time;
    }
    
    std::vector<unsigned char> msg = hexStr2BytesFast(time);
    const std::vector<unsigned char>& k = key_bytes;
    
    const EVP_MD* md = getHashAlgorithm(crypto);
    
    std::vector<unsigned char> hash(EVP_MAX_MD_SIZE);
    unsigned int hashLen = 0;
    
    HMAC(md, k.data(), k.size(), msg.data(), msg.size(),
            hash.data(), &hashLen);
    hash.resize(hashLen);
    
    int offset = hash[hashLen - 1] & 0x0F;

    int binary = ((hash[offset] & 0x7F) << 24) |
                    ((hash[offset + 1] & 0xFF) << 16) |
                    ((hash[offset + 2] & 0xFF) << 8) |
                    (hash[offset + 3] & 0xFF);

    long long otp = binary % DIGITS_POWER[codeDigits];

    std::string result = std::to_string(otp);
    if (result.length() < static_cast<size_t>(codeDigits)) 
    {
        result = std::string(codeDigits - result.length(), '0') + result;
    }
    
    return result;
}

std::string TOTPGenerator::base32Encode(const std::vector<unsigned char>& data) 
{
    static const char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    std::string result;
    result.reserve((data.size() + 4) / 5 * 8);
    
    uint64_t buffer = 0;
    int bitsLeft = 0;
    
    for (unsigned char byte : data) {
        buffer = (buffer << 8) | byte;
        bitsLeft += 8;
        
        while (bitsLeft >= 5) {
            bitsLeft -= 5;
            int index = (buffer >> bitsLeft) & 0x1F;
            result.push_back(alphabet[index]);
        }
    }
    
    if (bitsLeft > 0) {
        int index = (buffer << (5 - bitsLeft)) & 0x1F;
        result.push_back(alphabet[index]);
        result.append(8 - ((data.size() * 8 + 4) / 5), '=');
    } else {
        result.append((8 - result.size()) % 8, '=');
    }
    
    return result;
}

std::vector<unsigned char> TOTPGenerator::base32Decode(const std::string& data) 
{
    static const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    std::array<int, 256> rev{};
    rev.fill(-1);
    for (size_t i = 0; i < alphabet.size(); ++i) {
        rev[static_cast<unsigned char>(alphabet[i])] = static_cast<int>(i);
    }
    
    size_t end = data.size();
    int padding = 0;
    while (end > 0 && data[end - 1] == '=') 
    {
        ++padding;
        --end;
    }
    
    for (size_t i = 0; i < end; ++i) 
    {
        if (data[i] == '=') 
        {
            throw std::invalid_argument("Invalid base32: '=' not at the end");
        }
    }
    
    const size_t total_len = end + padding;
    if (padding > 0) 
    {
        if (total_len % 8 != 0) 
        {
            throw std::invalid_argument("Invalid base32: total length not multiple of 8");
        }
        if (padding != 1 && padding != 3 && padding != 4 && padding != 6) 
        {
            throw std::invalid_argument("Invalid base32: incorrect padding count");
        }
    }
    
    std::vector<unsigned char> result;
    uint64_t buffer = 0;
    int bits_left = 0;
    
    for (size_t i = 0; i < end; ++i) 
    {
        char c = data[i];
        int value = rev[static_cast<unsigned char>(c)];
        if (value == -1) 
        {
            throw std::invalid_argument("Invalid base32 character");
        }
        
        buffer = (buffer << 5) | value;
        bits_left += 5;
        
        if (bits_left >= 8) {
            bits_left -= 8;
            result.push_back(static_cast<unsigned char>((buffer >> bits_left) & 0xFF));
        }
    }
    
    if (bits_left >= 5) 
    {
        throw std::invalid_argument("Invalid base32: leftover bits >= 5");
    }
    if (bits_left > 0) 
    {
        uint64_t mask = (1ULL << bits_left) - 1;
        if ((buffer & mask) != 0) 
        {
            throw std::invalid_argument("Invalid base32: non-zero leftover bits");
        }
    }
    
    return result;
}

std::string TOTPGenerator::generateTOTPSecret(size_t keyLengthBytes) 
{
    std::vector<unsigned char> randomBytes(keyLengthBytes);
    
    if (RAND_bytes(randomBytes.data(), randomBytes.size()) != 1) 
    {
        throw std::runtime_error("Failed to generate random bytes");
    }
    
    return base32Encode(randomBytes);
}

std::string TOTPGenerator::passHash(const std::string& password, const std::string& crypto) 
{
    const int SALT_SIZE = 16;
    std::vector<unsigned char> salt(SALT_SIZE); 
    if (RAND_bytes(salt.data(), SALT_SIZE) != 1) 
    {
        throw std::runtime_error("RAND_bytes failed");
    }

    const int KEYLEN = 32;
    std::vector<unsigned char> hash(KEYLEN);

    if (PKCS5_PBKDF2_HMAC(
            password.c_str(), password.size(), 
            salt.data(), salt.size(), 
            600000,                                         // итерации
            TOTPGenerator::getHashAlgorithm(crypto),        // алгоритм хеширования
            KEYLEN,                                         // желаемая длина выхода
            hash.data()                                     // куда записать
        ) != 1) {
        throw std::runtime_error("PBKDF2 failed");
    }

    std::stringstream ss;
    for (unsigned char b : salt) ss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    ss << ":";
    for (unsigned char b : hash) ss << std::hex << std::setw(2) << std::setfill('0') << (int)b;

    return ss.str();
}

std::string TOTPGenerator::generateRandomPass() noexcept
{
    static const std::string alphabet = 
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> randSymbol(0, alphabet.size() - 1);

    std::string result;
    const int PASS_LEN = 10;
    for (int i = 0; i < PASS_LEN; ++i) 
    {
        auto new_char = alphabet[randSymbol(gen)];
        result.push_back(new_char);
    }
    
    return result; 
}

std::string TOTPGenerator::getHexUnixTime(unsigned long long unix_time)
{
    std::stringstream stream;
    stream << std::hex << std::setw(16) << std::setfill('0') << unix_time;
    return stream.str();
}
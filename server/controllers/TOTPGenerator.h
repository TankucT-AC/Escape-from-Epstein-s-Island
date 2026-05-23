// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TOTP_GENERATOR_H
#define TOTP_GENERATOR_H

#include <cstddef>
#include <string>
#include <vector>
#include <openssl/hmac.h>
#include <openssl/evp.h>

class TOTPGenerator 
{
private:
    const EVP_MD* getHashAlgorithm(const std::string& crypto);
    
public:
    std::string generate(const std::string& key,
                        std::string time,
                        const std::string& returnDigits,
                        const std::string& crypto);
    
    std::string generate(const std::vector<unsigned char>& key_bytes,
                        std::string time,
                        const std::string& returnDigits,
                        const std::string& crypto);

    std::string base32Encode(const std::vector<unsigned char>& data);
    std::vector<unsigned char> base32Decode(const std::string& data);

    std::string generateTOTPSecret(size_t keyLengthBytes = 20);

    std::string passHash(const std::string& password, const std::string& crypto = "HmacSHA256");
    std::string generateRandomPass() noexcept;
    std::string getHexUnixTime(unsigned long long unix_time);
};

#endif